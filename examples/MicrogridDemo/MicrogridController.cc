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

namespace Sgt
{
    void MicrogridController::setBuildBus(std::shared_ptr<SimBusAbc> bus)
    {
        buildBus_ = bus;
        dependsOn(buildBus_);
        buildBus_->didUpdate().addAction([this](){needsUpdate().trigger();}, "trigger " + id() + " needsUpdate()");
    }

    void MicrogridController::setPvBus(std::shared_ptr<SimBusAbc> bus)
    {
        pvBus_ = bus;
        dependsOn(pvBus_);
        pvBus_->didUpdate().addAction([this](){needsUpdate().trigger();}, "trigger " + id() + " needsUpdate()");
    }

    void MicrogridController::setBatt(std::shared_ptr<Battery> batt)
    {
        batt_ = batt;
        batt_->dependsOn(shared<MicrogridController>());
        didUpdate().addAction([this](){batt_->needsUpdate().trigger();}, "trigger " + batt_->id() + " needsUpdate()");
    }

    void MicrogridController::updateState(Time t)
    {
        double P = (buildBus_->bus()->SZip()(0) + pvBus_->bus()->SZip()(0)).real();
        if (P < minP_)
        {
            double battP = 0.5 * (maxP_ + minP_) - P;
            batt_->setRequestedPower(battP);
        }
        else if (P > maxP_)
        {
            double battP = 0.5 * (maxP_ + minP_) - P;
            batt_->setRequestedPower(battP);
        }
        else
        {
            batt_->setRequestedPower(0.0);
        }
    }

    void MicrogridControllerParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        string id = parser.expand<std::string>(nd["id"]);
        auto contr = sim.newSimComponent<MicrogridController>(id);

        contr->setMinP(parser.expand<double>(nd["min_P"]));
        contr->setMaxP(parser.expand<double>(nd["max_P"]));
        
        id = parser.expand<std::string>(nd["building_bus"]);
        contr->setBuildBus(sim.simComponent<SimBusAbc>(id));

        id = parser.expand<std::string>(nd["pv_bus"]);
        contr->setPvBus(sim.simComponent<SimBusAbc>(id));
        
        id = parser.expand<std::string>(nd["battery"]);
        contr->setBatt(sim.simComponent<Battery>(id));
    }
}

