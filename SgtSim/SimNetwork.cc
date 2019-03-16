// Copyright 2015-2016 National ICT Australia Limited (NICTA)
// Copyright 2016-2019 The Australian National University
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

#include "SimNetwork.h"


namespace Sgt
{
    SimNetwork::SimNetwork(const std::string& id, std::shared_ptr<Network> network) : 
        Component(id),
        network_(network),
        networkChanged_("SimNetwork network changed")
    {
        needsUpdate().addTrigger(networkChanged_);
    }

    void SimNetwork::initializeState()
    {
        networkChanged_.clearTriggers();
        for (auto branch: network_->branches())
        {
            networkChanged_.addTrigger(branch->admittanceChanged());
            networkChanged_.addTrigger(branch->isInServiceChanged());
        }
        for (auto bus: network_->buses())
        {
            networkChanged_.addTrigger(bus->setpointChanged());
            networkChanged_.addTrigger(bus->isInServiceChanged());
        }
        for (auto gen: network_->gens())
        {
            networkChanged_.addTrigger(gen->setpointChanged());
            networkChanged_.addTrigger(gen->isInServiceChanged());
        }
        for (auto zip: network_->zips())
        {
            networkChanged_.addTrigger(zip->injectionChanged());
            networkChanged_.addTrigger(zip->setpointChanged());
            networkChanged_.addTrigger(zip->isInServiceChanged());
        }
    }

    void SimNetwork::updateState(const Time& t)
    {
        sgtLogDebug() << "SimNetwork : update state." << std::endl;
        network_->solvePowerFlow(); // TODO: inefficient to rebuild even if not needed.
    }
}
