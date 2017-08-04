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
        std::string netwId = parser.expand<std::string>(nd["sim_network_id"]);
        auto simNetw = sim.simComponent<SimNetwork>(netwId);
        auto& netw = simNetw->network();

        mpParser.parse(nd, netw, parser);

        // Link to the SimNetwork.
        for (auto branch : netw.branches())
        {
            simNetw->linkBranch(*branch);
        }
        for (auto bus : netw.buses())
        {
            simNetw->linkBus(*bus);
        }
        for (auto gen : netw.gens())
        {
            simNetw->linkGen(*gen);
        }
        for (auto zip : netw.zips())
        {
            simNetw->linkZip(*zip);
        }
    }
}
