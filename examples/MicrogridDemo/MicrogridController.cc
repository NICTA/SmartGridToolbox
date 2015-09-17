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

#include "gurobi_c++.h"

namespace Sgt
{
    void MicrogridController::setBatt(std::shared_ptr<Battery> batt)
    {
        batt_ = batt;
        batt_->dependsOn(shared<MicrogridController>());
        didUpdate().addAction([this](){batt_->needsUpdate().trigger();}, "trigger " + batt_->id() + " needsUpdate()");
    }

    void MicrogridController::updateState(Time t)
    {
        const int N = 100;

        Time t0 = lastUpdated();
        std::vector<double> PLoad; PLoad.reserve(N);
        int dtSecs = 5 * 60;
        for (int i = 0; i < N; ++i)
        {
            Time ti = t0 + posix_time::seconds(i * dtSecs);
            PLoad.push_back(-real(loadSeries_->value(ti)[0])); // Change from inj. to draw.
        }
        double chg0 = batt_->charge();

        GRBEnv env = GRBEnv();
        GRBModel model = GRBModel(env);

        std::vector<GRBVar> PChg(N);
        std::vector<GRBVar> PDis(N);
        std::vector<GRBVar> chg(N);

        for (size_t i = 0; i < N; ++i)
        {
            PChg[i] =  model.addVar(0.0, batt_->maxChargePower(), 1.0, GRB_CONTINUOUS, "x");
            PDis[i] =  model.addVar(0.0, batt_->maxDischargePower(), 1.0, GRB_CONTINUOUS, "x");
        }
        model.update();
            
        GRBLinExpr obj; 
        for (size_t i = 0; i < N; ++i)
        {
            obj += PChg[i] + PLoad[i] - PDis[i]; // Electricity purchased from grid.
        }
        model.setObjective(obj);

        model.addConstr(chg[2] - chg0 - 2 * (PChg[1] - PDis[1]) * dtSecs == 0);
        for (size_t i = 1; i < N; ++i)
        {
            model.addConstr(chg[i + 1] - chg[i - 1] - 2 * (PChg[i] - PDis[i]) * dtSecs == 0);
        }

        model.update();
    }

    void MicrogridControllerParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        string id = parser.expand<std::string>(nd["id"]);
        auto contr = sim.newSimComponent<MicrogridController>(id);
        
        id = parser.expand<std::string>(nd["battery"]);
        contr->setBatt(sim.simComponent<Battery>(id));

        id = parser.expand<std::string>(nd["load_series"]);
        contr->setLoadSeries(sim.timeSeries<MicrogridController::TimeSeriesType>(id));
    }
}

