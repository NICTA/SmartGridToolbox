// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "MicrogridController.h"

#include <stdlib.h>

namespace Sgt
{
    MicrogridController::MicrogridController(const std::string& id, const Time& dt) :
        Component(id), Heartbeat(id, dt) 
    {
        int error = GRBloadenv(&env, "gurobi.log");
        sgtAssert(error == 0, "Gurobi exited with error " << error);
    }

    void MicrogridController::setBatt(std::shared_ptr<Battery> batt)
    {
        batt_ = batt;
        batt_->dependsOn(shared<MicrogridController>());
        didUpdate().addAction([this](){batt_->needsUpdate().trigger();}, "trigger " + batt_->id() + " needsUpdate()");
    }

    void MicrogridController::updateState(Time t)
    {
        // Variables:
        // [0 ... N - 1] : PChg[i] [0, maxPChg]
        // [N ... 2N - 1] : PDis[i] [0, min(maxPDis, PLd[i])] No feed in tariff so may as well not discharge more.
        // [2N ... 3N - 1] : Chg[i] [0, maxChg]
        //
        // Constraints:
        // Chg[0] = Chg0
        // Chg[i+1] - Chg[i] - chg_eff * dt * PChg[i] + (1 / dis_eff) * dt * PDis[i] = 0
        //
        // Objective
        // Sum_i price[i] * {PLd[i] + PChg[i] - PDis[i]} [= electricity purchased from grid]
        
        sgtLogDebug() << "MicrogridController" << std::endl; LogIndent _;

        const int N = 100;
        const int dtSecs = 15 * 60; // 15 minute steps.
        const double dtHrs = dtSecs / 3600.0;

        int iPImp[N];
        int iPExp[N];
        int iPChg[N];
        int iPDis[N];
        int iChg[N];
        for (int i = 0; i < N; ++i)
        {
            iPImp[i] = i;
            iPExp[i] = N + i;
            iPChg[i] = 2 * N + i;
            iPDis[i] = 3 * N + i;
            iChg[i] = 4 * N + i;
        }

        Time t0 = t; // We're doing a lookahead to set current operating params...
        std::vector<double> PRequired; PRequired.reserve(N); // Solar and load, as a draw.
        std::vector<double> price; price.reserve(N);
        for (int i = 0; i < N; ++i)
        {
            Time ti = t0 + posix_time::seconds(i * dtSecs);
            PRequired.push_back(-(solar_->PDc(ti) + real(loadSeries_->value(ti)[2]))); // Change from inj. to draw.
            price.push_back(priceSeries_->value(ti));
        }

        double chg0 = batt_->charge();

        GRBmodel* model = NULL;
        int error = 0;

        const int nVar = 5 * N;
        double obj[nVar];
        double lb[nVar];
        double ub[nVar];
        char vtype[nVar];
        char varnames1[nVar][32];
        char* varnames[nVar];

        for (int i = 0; i < N; ++i)
        {
            // PImp:
            obj[iPImp[i]] = price[i];
            lb[iPImp[i]] = 0.0;
            ub[iPImp[i]] = INFINITY;
            vtype[iPImp[i]] = GRB_CONTINUOUS;
            sprintf(varnames1[iPImp[i]], "P_imp_%d", i);
            varnames[iPImp[i]] = varnames1[iPImp[i]];
            
            // PExp:
            obj[iPExp[i]] = -feedInTariff_;
            lb[iPExp[i]] = 0.0;
            ub[iPExp[i]] = INFINITY;
            vtype[iPExp[i]] = GRB_CONTINUOUS;
            sprintf(varnames1[iPExp[i]], "P_exp_%d", i);
            varnames[iPExp[i]] = varnames1[iPExp[i]];

            // PChg:
            obj[iPChg[i]] = 0.0;
            lb[iPChg[i]] = 0.0;
            ub[iPChg[i]] = batt_->maxChargePower();
            vtype[iPChg[i]] = GRB_CONTINUOUS;
            sprintf(varnames1[iPChg[i]], "P_chg_%d", i);
            varnames[iPChg[i]] = varnames1[iPChg[i]];

            // PDis:
            obj[iPDis[i]] = 0.0;
            lb[iPDis[i]] = 0.0;
            ub[iPDis[i]] = batt_->maxDischargePower();
            vtype[iPDis[i]] = GRB_CONTINUOUS;
            sprintf(varnames1[iPDis[i]], "P_dis_%d", i);
            varnames[iPDis[i]] = varnames1[iPDis[i]];
            
            // Chg:
            obj[iChg[i]] = 0.0;
            lb[iChg[i]] = 0.0;
            ub[iChg[i]] = batt_->maxCharge();
            vtype[iChg[i]] = GRB_CONTINUOUS;
            sprintf(varnames1[iChg[i]], "chg_%d", i);
            varnames[iChg[i]] = varnames1[iChg[i]];
        }

        error = GRBnewmodel(env, &model, "gurobi_model", nVar, obj, lb, ub, vtype, varnames);
        sgtAssert(error == 0, "Gurobi exited with error " << error);
        error = GRBsetintattr(model, "ModelSense", 1);
        sgtAssert(error == 0, "Gurobi exited with error " << error);
        error = GRBupdatemodel(model);
        sgtAssert(error == 0, "Gurobi exited with error " << error);

        // Add constraints:
        {
            // Chg[0] = Chg0
            int constrInds[] = {iChg[0]};
            double constrVals[] = {1.0};
            char buff[32]; sprintf(buff, "constr_chg_%d", 0);
            error = GRBaddconstr(model, 1, constrInds, constrVals, GRB_EQUAL, chg0, buff);
            sgtAssert(error == 0, "Gurobi exited with error " << error);
        }

        {
            // Chg[i+1] - Chg[i] - chg_eff * dt * PChg[i] + (1 / dis_eff) * dt * PDis[i] = 0
            double chgFactor = -dtHrs * batt_->chargeEfficiency();
            double disFactor = dtHrs / batt_->dischargeEfficiency();
            for (int i = 0; i < N - 1; ++i)
            {
                int constrInds[] = {iChg[i + 1], iChg[i], iPChg[i], iPDis[i]};
                double constrVals[] = {1.0, -1.0, chgFactor, disFactor};
                char buff[32]; sprintf(buff, "constr_chg_%d", i + 1);
                error = GRBaddconstr(model, 4, constrInds, constrVals, GRB_EQUAL, 0.0, buff);
                sgtAssert(error == 0, "Gurobi exited with error " << error);
            }
        }

        // PImp + PDis - PExp - PChg = PRequired
        for (int i = 0; i < N - 1; ++i)
        {
            int constrInds[] = {iPImp[i], iPDis[i], iPExp[i], iPChg[i]};
            double constrVals[] = {1.0, 1.0, -1.0, -1.0};
            char buff[32]; sprintf(buff, "constr_P_bal_%d", i);
            error = GRBaddconstr(model, 4, constrInds, constrVals, GRB_EQUAL, PRequired[i], buff);
            sgtAssert(error == 0, "Gurobi exited with error " << error);
        }
        
        error = GRBupdatemodel(model);
        sgtAssert(error == 0, "Gurobi exited with error " << error);
        
        error = GRBoptimize(model);
        sgtAssert(error == 0, "Gurobi exited with error " << error);
        
        // Retrieve the results:
        double PImp[N];
        error = GRBgetdblattrarray(model, "X", 0, N, PImp);
        double PExp[N];
        error = GRBgetdblattrarray(model, "X", N, N, PExp);
        double PChg[N];
        error = GRBgetdblattrarray(model, "X", 2 * N, N, PChg);
        double PDis[N];
        error = GRBgetdblattrarray(model, "X", 3 * N, N, PDis);
        double chg[N];
        error = GRBgetdblattrarray(model, "X", 4 * N, N, chg);

        batt_->setRequestedPower(PDis[0] - PChg[0]); // Injection.
        
        Heartbeat::updateState(t);
    }

    void MicrogridControllerParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        string id = parser.expand<std::string>(nd["id"]);
        Time dt = posix_time::minutes(5);
        auto contr = sim.newSimComponent<MicrogridController>(id, dt);
        
        auto ndDt = nd["dt"];
        if (ndDt)
        {
            dt = parser.expand<Time>(ndDt);
            contr->setDt(dt);
        }

        id = parser.expand<std::string>(nd["battery"]);
        contr->setBatt(sim.simComponent<Battery>(id));

        id = parser.expand<std::string>(nd["load_series"]);
        contr->setLoadSeries(sim.timeSeries<MicrogridController::LoadSeries>(id));
        
        id = parser.expand<std::string>(nd["price_series"]);
        contr->setPriceSeries(sim.timeSeries<MicrogridController::PriceSeries>(id));
        
        id = parser.expand<std::string>(nd["solar"]);
        contr->setSolar(sim.simComponent<SolarPv>(id));
        
        double feedInTariff = parser.expand<double>(nd["feed_in_tariff"]);
        contr->setFeedInTariff(feedInTariff);
    }
}
