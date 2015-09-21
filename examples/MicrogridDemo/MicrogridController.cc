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
        const int nVar = 3 * N;
        const int dtSecs = 15 * 60;
        const double dtHrs = dtSecs / 3600.0;
        int iPChg[N];

        int iPDis[N];
        int iChg[N];
        for (int i = 0; i < N; ++i)
        {
            iPChg[i] = i;
            iPDis[i] = N + i;
            iChg[i] = 2 * N + i;
        }

        Time t0 = t; // We're doing a lookahead to set current operating params...
        std::vector<double> PRequired; PRequired.reserve(N); // Load minus solar.
        std::vector<double> price; price.reserve(N);
        for (int i = 0; i < N; ++i)
        {
            Time ti = t0 + posix_time::seconds(i * dtSecs);
            PRequired.push_back(-real(loadSeries_->value(ti)[2]) - solar_->PDc(ti)); // Change from inj. to draw.
            price.push_back(priceSeries_->value(ti));
        }

        double chg0 = batt_->charge();

        GRBmodel* model = NULL;
        int error = 0;

        double obj[nVar];
        double lb[nVar];
        double ub[nVar];
        char vtype[nVar];
        char varnames1[nVar][16];
        char* varnames[nVar];
        for (int i = 0; i < N; ++i)
        {
            // PChg:
            obj[iPChg[i]] = price[i];
            lb[iPChg[i]] = 0.0;
            ub[iPChg[i]] = batt_->maxChargePower();
            vtype[iPChg[i]] = GRB_CONTINUOUS;
            sprintf(varnames1[iPChg[i]], "PChg_%d", i);
            varnames[iPChg[i]] = varnames1[iPChg[i]];

            // PDis:
            obj[iPDis[i]] = -price[i];
            lb[iPDis[i]] = 0.0;
            ub[iPDis[i]] = std::min(batt_->maxDischargePower(), PRequired[i]);
            vtype[iPDis[i]] = GRB_CONTINUOUS;
            sprintf(varnames1[iPDis[i]], "PDis_%d", i);
            varnames[iPDis[i]] = varnames1[iPDis[i]];
            
            // Chg:
            obj[iChg[i]] = 0.0;
            lb[iChg[i]] = 0.0;
            ub[iChg[i]] = batt_->maxCharge();
            vtype[iChg[i]] = GRB_CONTINUOUS;
            sprintf(varnames1[iChg[i]], "Chg_%d", i);
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
            char buff[16]; sprintf(buff, "constr_chg_%d", 0);
            error = GRBaddconstr(model, 1, constrInds, constrVals, GRB_EQUAL, chg0, buff);
            sgtAssert(error == 0, "Gurobi exited with error " << error);
        }
        double chgFactor = -dtHrs * batt_->chargeEfficiency();
        double disFactor = dtHrs / batt_->dischargeEfficiency();
        for (int i = 0; i < N - 1; ++i)
        {
            // Chg[i+1] - Chg[i] - chg_eff * dt * PChg[i] + (1 / dis_eff) * dt * PDis[i] = 0
            int constrInds[] = {iChg[i + 1], iChg[i], iPChg[i], iPDis[i]};
            double constrVals[] = {1.0, -1.0, chgFactor, disFactor};
            char buff[16]; sprintf(buff, "constr_chg_%d", i + 1);
            error = GRBaddconstr(model, 4, constrInds, constrVals, GRB_EQUAL, 0.0, buff);
            sgtAssert(error == 0, "Gurobi exited with error " << error);
        }
        
        error = GRBupdatemodel(model);
        sgtAssert(error == 0, "Gurobi exited with error " << error);
        
        error = GRBoptimize(model);
        sgtAssert(error == 0, "Gurobi exited with error " << error);
        
        // Retrieve the results:
        double PChg[N];
        error = GRBgetdblattrarray(model, "X", 0, N, PChg);
        double PDis[N];
        error = GRBgetdblattrarray(model, "X", N, N, PDis);
        double chg[N];
        error = GRBgetdblattrarray(model, "X", 2 * N, N, chg);

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
    }
}
