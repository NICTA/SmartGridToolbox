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

#include "SimBranchParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/Branch.h>

namespace Sgt
{
    void SimBranchParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "network_id");
        
        string id = parser.expand<std::string>(nd["id"]);

        string netwId = parser.expand<std::string>(nd["network_id"]);
        auto network = sim.simComponent<SimNetwork>(netwId)->network();

        NetworkParser p;
        p.parse(nd, *network);
        auto branch = network->branch("id");

        sim.newSimComponent<SimBranch>(branch);
    }
}
