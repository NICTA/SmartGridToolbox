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

#include "SimYyTransformerParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/YyTransformer.h>
#include <SgtCore/YyTransformerParserPlugin.h>

#include <memory>

namespace Sgt
{
    void SimYyTransformerParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        YyTransformerParserPlugin transParser;
        auto trans = sim.newSimComponent<SimYyTransformer>(*transParser.parseYyTransformer(nd, parser));

        assertFieldPresent(nd, "network_id");
        assertFieldPresent(nd, "bus_0_id");
        assertFieldPresent(nd, "bus_1_id");

        string netwId = parser.expand<std::string>(nd["network_id"]);
        std::string bus0Id = parser.expand<std::string>(nd["bus_0_id"]);
        std::string bus1Id = parser.expand<std::string>(nd["bus_1_id"]);

        auto netw = sim.simComponent<SimNetwork>(netwId);

        netw->addBranch(trans, bus0Id, bus1Id);
    }
}
