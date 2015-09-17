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

extern "C" {
#include "gurobi_c.h"
};

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

        GRBenv* env = NULL;
        GRBmodel* model = NULL;
        int error = 0;

        error = GRBloadenv(&env, "gurobi.log"); assert(error == 0);

        double obj[3 * N];
        double lb[3 * N];
        double ub[3 * N];
        char vtype[3 * N];
        for (int i = 0; i < N; ++i)
        {
            obj[i] = 1.0;
            lb[i] = 0.0;
            ub[i] = batt_->maxChargePower();
            vtype[i] = GRB_CONTINUOUS;

            obj[N + i] = -1.0;
            lb[N + i] = 0.0;
            ub[N + i] = batt_->maxDischargePower();
            vtype[N + i] = GRB_CONTINUOUS;
            
            obj[2 * N + i] = 0.0;
            lb[2 * N + i] = -INFINITY;
            ub[2 * N + i] = INFINITY;
            vtype[2 * N + i] = GRB_CONTINUOUS;
        }

        error = GRBnewmodel(env, &model, "gurobi_model", 3 * N, obj, lb, ub, vtype, NULL); assert(error == 0);

        error = GRBupdatemodel(model); assert(error == 0);

        int constrInds[4];
        double constrVals[4];

        constrInds[0] = 2 * N + 2; constrInds[1] = 1; constrInds[2] = N + 1;
        constrVals[0] = 1; constrVals[1] = 2 * dtSecs; constrVals[2] = -2 * dtSecs;
        GRBaddconstr(model, 3, constrInds, constrVals, GRB_EQUAL, chg0, NULL);
        for (int i = 1; i < N; ++i)
        {
            constrInds[0] = 2 * N + i + 1; constrInds[2] = 2 * N + i - 1; constrInds[2] = i; constrInds[3] = N + i;
            constrVals[0] = 1; constrVals[1] = -1; constrVals[2] = -2 * dtSecs; constrVals[3] = 2 * dtSecs;
            GRBaddconstr(model, 3, constrInds, constrVals, GRB_EQUAL, 0.0, NULL);
        }

        GRBsetintattr(model, "ModelSense", 1);

        GRBoptimize(model);
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

