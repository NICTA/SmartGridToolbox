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

#include "SimNetwork.h"


namespace Sgt
{
    SimNetwork::SimNetwork(const std::string& id, std::shared_ptr<Network> network) : 
        Component(id), network_(network)
    {
        for (auto branch: network->branches())
        {
            linkBranch(*branch);
        }
        for (auto bus: network->buses())
        {
            linkBus(*bus);
        }
        for (auto gen: network->gens())
        {
            linkGen(*gen);
        }
        for (auto zip: network->zips())
        {
            linkZip(*zip);
        }
    }

    void SimNetwork::linkBranch(const BranchAbc& branch)
    {
        needsUpdate().addTrigger(branch.admittanceChanged());
        needsUpdate().addTrigger(branch.isInServiceChanged());
    }

    void SimNetwork::linkBus(const Bus& bus)
    {
        needsUpdate().addTrigger(bus.setpointChanged());
        needsUpdate().addTrigger(bus.isInServiceChanged());
    }

    void SimNetwork::linkGen(const Gen& gen)
    {
        needsUpdate().addTrigger(gen.setpointChanged());
        needsUpdate().addTrigger(gen.isInServiceChanged());
    }

    void SimNetwork::linkZip(const Zip& zip)
    {
        needsUpdate().addTrigger(zip.injectionChanged());
        needsUpdate().addTrigger(zip.setpointChanged());
        needsUpdate().addTrigger(zip.isInServiceChanged());
    }

    void SimNetwork::updateState(Time t)
    {
        sgtLogDebug() << "SimNetwork : update state." << std::endl;
        network_->solvePowerFlow(); // TODO: inefficient to rebuild even if not needed.
    }

    SimBranch::SimBranch(const ComponentPtr<BranchAbc>& branch, SimNetwork& simNetwork) : branch_(branch)
    {
        simNetwork.linkBranch(*branch);
    }
            
    SimBus::SimBus(const ComponentPtr<Bus>& bus, SimNetwork& simNetwork) : bus_(bus)
    {
        simNetwork.linkBus(*bus);
    }

    SimGen::SimGen(const ComponentPtr<Gen>& gen, SimNetwork& simNetwork) : gen_(gen)
    {
        simNetwork.linkGen(*gen);
    }

    SimZip::SimZip(const ComponentPtr<Zip>& zip, SimNetwork& simNetwork) : zip_(zip)
    {
        simNetwork.linkZip(*zip);
    }
}
