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

#include "SimGenParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/Gen.h>
#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    void SimGenParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "sim_network_id");
        assertFieldPresent(nd, "bus_id");
        assertFieldPresent(nd, "gen");
        
        string id = parser.expand<std::string>(nd["id"]);
        string simNetwId = parser.expand<std::string>(nd["sim_network_id"]);

        auto simNetwork = sim.simComponent<SimNetwork>(simNetwId);
        sgtAssert(simNetwork != nullptr, "Parsing SimGen " << id << ": sim_network not found.");
        auto& network = simNetwork->network();

        YAML::Node genNode = nd["gen"];

        YAML::Node netwNode;
        netwNode.push_back(genNode);
        NetworkParser p = parser.subParser<Network>();
        p.parse(netwNode, network);
        auto& gens = network.gens();
        auto gen = gens.back(); // KLUDGE: assumes just parsed gen is last (which is true, but not robust).
        auto simGen = sim.newSimComponent<SimGen>(id, *gen);
        simGen->linkToSimNetwork(*simNetwork);
    }
}
