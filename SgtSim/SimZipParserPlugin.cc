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

#include "SimZipParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/Zip.h>
#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    void SimZipParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "sim_network_id");
        assertFieldPresent(nd, "bus_id");
        assertFieldPresent(nd, "zip");
        
        string id = parser.expand<std::string>(nd["id"]);
        string simNetwId = parser.expand<std::string>(nd["sim_network_id"]);

        auto simNetwork = sim.simComponents()[simNetwId].as<SimNetwork>();
        sgtAssert(simNetwork != nullptr, "Parsing SimZip " << id << ": sim_network not found.");
        auto& network = simNetwork->network();

        YAML::Node zipNode = nd["zip"];

        YAML::Node netwNode;
        netwNode.push_back(zipNode);
        NetworkParser p = parser.subParser<Network>();
        p.parse(netwNode, network);
        auto zip = *network.zips().rbegin(); // KLUDGE: assumes just parsed zip is last (true, but not robust).
        auto simZip = std::make_shared<SimZip>(id, *zip);
        sim.addSimComponent(simZip);
        simZip->linkToSimNetwork(*simNetwork);
    }
}
