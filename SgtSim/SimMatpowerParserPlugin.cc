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

#include "SimMatpowerParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/MatpowerParserPlugin.h>

namespace Sgt
{
    void SimMatpowerParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        MatpowerParserPlugin mpParser;
        assertFieldPresent(nd, "sim_network_id");
        string netwId = parser.expand<std::string>(nd["sim_network_id"]);
        auto simNetw = sim.simComponent<SimNetwork>(netwId);
        auto& netw = simNetw->network();

        mpParser.parse(nd, netw, parser);

        // Now recreate the SimNetwork from the Network.
        for (auto bus : netw.buses())
        {
            auto simBus = sim.newSimComponent<SimBus>(bus->id(), *bus);
            simBus->linkToSimNetwork(*simNetw);

            for (auto gen : bus->gens())
            {
                auto simGen = sim.newSimComponent<SimGen>(gen->id(), *gen);
                simGen->linkToSimNetwork(*simNetw);
            }

            for (auto zip : bus->zips())
            {
                auto simZip = sim.newSimComponent<SimZip>(zip->id(), *zip);
                simZip->linkToSimNetwork(*simNetw);
            }
        }

        for (auto branch : netw.branches())
        {
            auto simBranch = sim.newSimComponent<SimBranch>(branch->id(), *branch);
            simBranch->linkToSimNetwork(*simNetw);
        }
    }
}
