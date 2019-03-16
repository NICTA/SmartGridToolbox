// Copyright 2015-2016 National ICT Australia Limited (NICTA)
// Copyright 2016-2019 The Australian National University
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

using namespace std;

namespace Sgt
{
    void PvInverter::PChanged()
    {
        double PMax = requestedPDc();
        gen()->setPMax(PMax);
    }

    void PvInverterParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "sim_network_id");
        assertFieldPresent(nd, "bus_id");

        string id = parser.expand<std::string>(nd["id"]);
        const std::string busId = parser.expand<std::string>(nd["bus_id"]);

        const std::string networkId = parser.expand<std::string>(nd["sim_network_id"]);
        auto simNetw = sim.simComponent<SimNetwork>(networkId);
        auto gen = simNetw->network().addGen(std::make_shared<Gen>(id, Phase::BAL), busId);

        auto inverter = sim.newSimComponent<PvInverter>(id, gen);

        if (nd["efficiency"])
        {
            inverter->setEfficiencyDcToAc(parser.expand<double>(nd["efficiency"]));
            inverter->setEfficiencyAcToDc(parser.expand<double>(nd["efficiency"]));
        }

        if (nd["max_S_mag"])
        {
            inverter->setMaxSMag(parser.expand<double>(nd["max_S_mag"]));
        }
        
        if (nd["max_Q"])
        {
            inverter->gen()->setQMax(parser.expand<double>(nd["max_Q"]));
        }

        simNetw->addSimGen(inverter);
    }
}
