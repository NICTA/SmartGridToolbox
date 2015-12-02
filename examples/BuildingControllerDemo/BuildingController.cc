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

#include "BuildingController.h"

#include "Building.h"

#include <stdlib.h>

namespace Sgt
{
    BuildingController::BuildingController(const std::string& id, const Time& dt) :
        Component(id), Heartbeat(id, dt) 
    {
        env_.set(GRB_IntParam_LogToConsole, 0);
    }

    void BuildingController::setBatt(Battery& batt)
    {
        batt_ = &batt;
        SimComponent::addDependency(batt, *this, false);
    }

    void BuildingController::setBuild(Building& build)
    {
        build_ = &build;
        SimComponent::addDependency(build, *this, false);
    }
            
    void BuildingController::setSolar(SolarPv& solar)
    {
        solar_ = &solar;
        SimComponent::addDependency(solar, *this, false);
    }

    void BuildingController::updateState(Time t)
    {
        assert(batt_->validUntil() > t); // Next update is in the future.
        assert(build_->validUntil() > t); // Next update is in the future.
        assert(solar_->validUntil() > t); // Next update is in the future.

        // Variables:
        // [0 ... N - 1] : PImp[i] [-Inf, Inf]
        // [N ... 2N - 1] : PExp[i] [-Inf, Inf]
        // [2N ... 3N - 1] : PChg[i] [0, maxPChg]
        // [3N ... 4N - 1] : PDis[i] [0, maxPDis]
        // [4N ... 5N - 1] : Chg[i] [0, maxChg]
        // [5N ... 6N - 1] : Tb[i] [-Infin, Infin]
        // [6N ... 7N - 1] : TbPlus[i] [0, TMaxDev_]
        // [7N ... 8N - 1] : TbMinus[i] [0, TMaxDev_]
        // [8N ... 9N - 1] : TbPlusPen[i] [0, Infin]
        // [9N ... 10N - 1] : TbMinusPen[i] [0, Infin]
        // [10N ... 11N - 1] : PCool[i] [0, maxPCool]
        // [11N ... 12N - 1] : PHeat[i] [0, maxPHeat]
        //
        // Constraints:
        // Chg[0] = Chg0
        // Chg[i+1] - Chg[i] - chg_eff * dt * PChg[i] + (1 / dis_eff) * dt * PDis[i] = 0
        // Tb[0] = Tb0
        // Tb[i+1] - d Tb[i] + ((1 - d) COPCool / kb) PCool[i] - ((1 - d) COPHeat / kb) PHeat = (1 - d) TExt
        // Tb[i] - TbPlus[i] + TbMinus[i] - TbPlusPen + TbMinusPen = TSetp_
        // PImp[i] + PDis[i] - PExp[i] - PChg[i] - PHeat[i] - PCool[i] = PUncontrolledLoad[i]
        //
        // Objective:
        // Sum_i (price[i] * PImp[i] - feedInTariff_ * PExp[i] + comfortFactor_ * (TbPlusPen[i] + TbMinusPen[i])
        
        // Parameters:
        const int N = 100;
        const int dtSecs = 15 * 60; // 15 minute steps.
        const double dtHrs = dtSecs / 3600.0;

        // Constants:
        std::vector<double> PUncontrolledLoad; PUncontrolledLoad.reserve(N); // Solar and load, as a draw.
        std::vector<double> price; price.reserve(N);
        std::vector<double> TExt; TExt.reserve(N);
        for (int i = 0; i < N; ++i)
        {
            Time ti = t + timeFromDSeconds((i + 0.5) * dtSecs); // Use midpoint of timestep for consts.
            PUncontrolledLoad.push_back(-(solar_->PDc(ti) + real(loadSeries_->value(ti)[2])));
                // Change from inj. to draw.
            price.push_back(priceSeries_->value(ti));
            TExt.push_back(TExtSeries_->value(ti));
        }

        double chg0 = batt_->charge();
        
        double Tb0 = build_->Tb();
        
        // Gurobi:
        GRBModel model(env_);

        std::vector<GRBVar> PImpVars; PImpVars.reserve(N);
        std::vector<GRBVar> PExpVars; PExpVars.reserve(N);
        std::vector<GRBVar> PChgVars; PChgVars.reserve(N);
        std::vector<GRBVar> PDisVars; PDisVars.reserve(N);
        std::vector<GRBVar> chgVars; chgVars.reserve(N);
        std::vector<GRBVar> TbVars; TbVars.reserve(N);
        std::vector<GRBVar> TbPlusVars; TbPlusVars.reserve(N);
        std::vector<GRBVar> TbMinusVars; TbMinusVars.reserve(N);
        std::vector<GRBVar> TbPlusPenVars; TbPlusPenVars.reserve(N);
        std::vector<GRBVar> TbMinusPenVars; TbMinusPenVars.reserve(N);
        std::vector<GRBVar> PCoolVars; PCoolVars.reserve(N);
        std::vector<GRBVar> PHeatVars; PHeatVars.reserve(N);

        // Add variables:
        for (size_t i = 0; i < N; ++i)
        {
            std::string iStr = std::to_string(i);

            PImpVars.push_back(model.addVar(0.0, INFINITY, price[i], GRB_CONTINUOUS, "P_imp_" + iStr));
            PExpVars.push_back(model.addVar(0.0, INFINITY, -feedInTariff_, GRB_CONTINUOUS, "P_exp_" + iStr));
            PChgVars.push_back(model.addVar(0.0, batt_->maxChargePower(), 0.0, GRB_CONTINUOUS, "P_chg_" + iStr));
            PDisVars.push_back(model.addVar(0.0, batt_->maxDischargePower(), 0.0, GRB_CONTINUOUS, "P_dis_" + iStr));
            chgVars.push_back(model.addVar(0.0, batt_->maxCharge(), 0.0, GRB_CONTINUOUS, "chg_" + iStr));
            TbVars.push_back(model.addVar(-INFINITY, INFINITY, 0.0, GRB_CONTINUOUS, "Tb_" + iStr));
            TbPlusVars.push_back(model.addVar(0, TMaxDev_, 0.0, GRB_CONTINUOUS, "TbPlus_" + iStr));
            TbMinusVars.push_back(model.addVar(0, TMaxDev_, 0.0, GRB_CONTINUOUS, "TbMinus_" + iStr));
            TbPlusPenVars.push_back(model.addVar(0, INFINITY, comfortFactor_, GRB_CONTINUOUS, "TbPlusPen_" + iStr));
            TbMinusPenVars.push_back(model.addVar(0, INFINITY, comfortFactor_, GRB_CONTINUOUS, "TbMinusPen_" + iStr));
            PCoolVars.push_back(model.addVar(0, build_->maxPCool(), 0.0, GRB_CONTINUOUS, "PCool_" + iStr));
            PHeatVars.push_back(model.addVar(0, build_->maxPHeat(), 0.0, GRB_CONTINUOUS, "PHeat_" + iStr));
        }

        model.set(GRB_IntAttr_ModelSense, 1);
        model.update();

        // Add constraints:
        
        // Chg[0] = Chg0
        model.addConstr(chgVars[0] == chg0, "constr_chg_0");

        // Chg[i+1] - Chg[i] - chg_eff * dt * PChg[i] + (1 / dis_eff) * dt * PDis[i] = 0
        double chgFactor = -dtHrs * batt_->chargeEfficiency();
        double disFactor = dtHrs / batt_->dischargeEfficiency();
        for (size_t i = 0; i < N - 1; ++i)
        {
            model.addConstr(chgVars[i+1] - chgVars[i] + chgFactor * PChgVars[i] + disFactor * PDisVars[i] == 0,
                    "constr_chg_" + std::to_string(i));
        }

        // Tb[0] = Tb0
        model.addConstr(TbVars[0] == Tb0, "constr_Tb_0");
        
        // Tb[i+1] - d Tb[i] + ((1 - d) COPCool / kb) PCool[i] - ((1 - d) COPHeat / kb) PHeat = (1 - d) TExt
        for (size_t i = 0; i < N - 1; ++i)
        {
            double d = build_->d(dtSecs);
            double d1 = 1.0 - d;
            double tmp = d1 / build_->kb();
            double PCoolFactor = tmp * build_->copCool();
            double PHeatFactor = -tmp * build_->copHeat();
        
            model.addConstr(TbVars[i+1] - d * TbVars[i] + PCoolFactor * PCoolVars[i] + PHeatFactor * PHeatVars[i] 
                    == d1 * TExt[i], "constr_Tb_" + std::to_string(i));
        }
        
        // Tb[i] - TbPlus[i] + TbMinus[i] - TbPlusPen + TbMinusPen = TSetp_
        for (size_t i = 0; i < N; ++i)
        {
            model.addConstr(TbVars[i] - TbPlusVars[i] + TbMinusVars[i] - TbPlusPenVars[i] + TbMinusPenVars[i]
                    == TSetp_, "constr_Tb_bal_" + std::to_string(i));
        }
        
        // PImp[i] + PDis[i] - PExp[i] - PChg[i] - PHeat[i] - PCool[i] = PUncontrolledLoad[i]
        for (size_t i = 0; i < N; ++i)
        {
            model.addConstr(PImpVars[i] + PDisVars[i] - PExpVars[i] - PChgVars[i] - PHeatVars[i] - PCoolVars[i]
                    == PUncontrolledLoad[i], "constr_P_bal_" + std::to_string(i));
        }
      
        model.update();
        model.optimize();

        // Propogate control to battery and building:
        batt_->setRequestedPower(PDisVars[0].get(GRB_DoubleAttr_X) - PChgVars[0].get(GRB_DoubleAttr_X)); // Injection.
        assert(PCoolVars[0].get(GRB_DoubleAttr_X) * PHeatVars[0].get(GRB_DoubleAttr_X) 
                < std::numeric_limits<double>::epsilon());
        build_->setReqPCool(PCoolVars[0].get(GRB_DoubleAttr_X)); // Injection.
        build_->setReqPHeat(PHeatVars[0].get(GRB_DoubleAttr_X)); // Injection.
        
        Heartbeat::updateState(t);
    }

    void BuildingControllerParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        string id = parser.expand<std::string>(nd["id"]);
        Time dt = posix_time::minutes(5);
        auto contr = sim.newSimComponent<BuildingController>(id, dt);
        
        auto ndDt = nd["dt"];
        if (ndDt)
        {
            dt = parser.expand<Time>(ndDt);
            contr->setDt(dt);
        }

        id = parser.expand<std::string>(nd["battery"]);
        contr->setBatt(*sim.simComponent<Battery>(id));
        
        id = parser.expand<std::string>(nd["building"]);
        contr->setBuild(*sim.simComponent<Building>(id));
        
        id = parser.expand<std::string>(nd["solar"]);
        contr->setSolar(*sim.simComponent<SolarPv>(id));

        id = parser.expand<std::string>(nd["load_series"]);
        contr->setLoadSeries(*sim.timeSeries<BuildingController::LoadSeries>(id));
        
        id = parser.expand<std::string>(nd["price_series"]);
        contr->setPriceSeries(*sim.timeSeries<BuildingController::PriceSeries>(id));
        
        id = parser.expand<std::string>(nd["T_extern_series"]);
        contr->setTExtSeries(*sim.timeSeries<BuildingController::TempSeries>(id));
        
        double feedInTariff = parser.expand<double>(nd["feed_in_tariff"]);
        contr->setFeedInTariff(feedInTariff);
        
        double comfortFactor = parser.expand<double>(nd["comfort_factor"]);
        contr->setComfortFactor(comfortFactor);
        
        double TSetp = parser.expand<double>(nd["T_setpoint"]);
        contr->setTSetp(TSetp);
        
        double TMaxDev = parser.expand<double>(nd["T_max_dev"]);
        contr->setTMaxDev(TMaxDev);
    }
}
