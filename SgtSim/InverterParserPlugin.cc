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
        assertFieldPresent(nd, "phases");
        assertFieldPresent(nd, "network_id");
        assertFieldPresent(nd, "bus_id");

        string id = parser.expand<std::string>(nd["id"]);
        Phases phases = parser.expand<Phases>(nd["phases"]);

        auto inverter = sim.newSimComponent<SimpleZipInverter>(id, phases);

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

        const std::string networkId = parser.expand<std::string>(nd["network_id"]);
        const std::string busId = parser.expand<std::string>(nd["bus_id"]);

        auto network = sim.simComponent<SimNetwork>(networkId);
        network->addZip(inverter, busId);
    }
}
