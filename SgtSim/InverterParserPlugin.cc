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

#include "InverterParserPlugin.h"

#include "Inverter.h"
#include "SimNetwork.h"
#include "Simulation.h"

namespace Sgt
{
    void InverterParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "sim_network_id");
        assertFieldPresent(nd, "bus_id");
        assertFieldPresent(nd, "phases");

        std::string id = parser.expand<std::string>(nd["id"]);
        std::string busId = parser.expand<std::string>(nd["bus_id"]);
        auto ndZipId = nd["zip_id"];
        std::string zipId = ndZipId ? parser.expand<std::string>(ndZipId) : id;
        std::string simNetworkId = parser.expand<std::string>(nd["sim_network_id"]);

        auto& simNetwork = *sim.simComponent<SimNetwork>(simNetworkId);
        Network& network = simNetwork.network();
        ComponentPtr<Zip> zip = network.zips()[zipId]; 
        if (zip == nullptr)
        {
            assertFieldPresent(nd, "phases");
            Phases phases = parser.expand<Phases>(nd["phases"]);
            zip = network.addZip(std::make_shared<Zip>(zipId, phases), busId);
        }

        auto inverter = sim.newSimComponent<Inverter>(id, zip, simNetwork);

        if (nd["efficiency"])
        {
            inverter->setEfficiency(parser.expand<double>(nd["efficiency"]));
        }

        if (nd["max_S_mag"])
        {
            inverter->setMaxSMag(parser.expand<double>(nd["max_S_mag"]));
        }

        if (nd["requested_Q"])
        {
            inverter->setRequestedQ(parser.expand<double>(nd["requested_Q"]));
        }
    }
}
