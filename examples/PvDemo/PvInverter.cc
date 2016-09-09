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

#include "PvInverter.h"

#include <SgtSim/Simulation.h>

namespace Sgt
{
    void PvInverter::PChanged()
    {
        double PMax = availableP();
        setPMax(PMax);
    }

    void PvInverterParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "sim_network_id");
        assertFieldPresent(nd, "bus_id");

        string id = parser.expand<std::string>(nd["id"]);
        const std::string busId = parser.expand<std::string>(nd["bus_id"]);

        auto inverter = sim.newSimComponent<PvInverter>(id);

        if (nd["efficiency"])
        {
            inverter->setEfficiency(parser.expand<double>(nd["efficiency"]));
        }

        if (nd["max_S_mag"])
        {
            inverter->setMaxSMag(parser.expand<double>(nd["max_S_mag"]));
        }
        
        if (nd["max_Q"])
        {
            inverter->setMaxQ(parser.expand<double>(nd["max_Q"]));
        }

        const std::string networkId = parser.expand<std::string>(nd["sim_network_id"]);
        auto network = sim.simComponent<SimNetwork>(networkId);
        network->network().addGen(shared<GenAbc>(inverter->gen()), busId);
        inverter->linkToSimNetwork(*network);
    }
}
