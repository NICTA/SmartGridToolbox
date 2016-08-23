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

#include "SimBusParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/Bus.h>
#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    void SimBusParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "sim_network_id");
        assertFieldPresent(nd, "bus");
        
        string id = parser.expand<std::string>(nd["id"]);
        string simNetwId = parser.expand<std::string>(nd["sim_network_id"]);

        auto simNetwork = sim.simComponents()[simNetwId].as<SimNetwork>();
        sgtAssert(simNetwork != nullptr, "Parsing SimBus " << id << ": sim_network not found.");
        auto& network = simNetwork->network();

        YAML::Node busNode = nd["bus"];

        YAML::Node netwNode;
        netwNode.push_back(busNode);
        NetworkParser p = parser.subParser<Network>();
        p.parse(netwNode, network);
        auto bus = *network.buses().rbegin(); // KLUDGE: assumes just parsed bus is last (true, but not robust).
        auto simBus = std::make_shared<SimBus>(id, *bus);
        sim.addSimComponent(simBus);
        simBus->linkToSimNetwork(*simNetwork);
    }
}
