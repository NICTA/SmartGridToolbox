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

#include "HeartbeatParserPlugin.h"

#include "Heartbeat.h"

#include <SgtSim/SimNetwork.h>
#include <SgtSim/Simulation.h>

#include <vector>

namespace Sgt
{
    void HeartbeatParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "dt");

        std::string id = parser.expand<std::string>(nd["id"]);
        Time dt = parser.expand<Time>(nd["dt"]);

        auto heartbeat = sim.newSimComponent<Heartbeat>(id, dt);
       
        auto ndSlaves = nd["slaves"];
        if (ndSlaves)
        {
            std::vector<std::string> slaves = parser.expand<std::vector<std::string>>(nd["slaves"]);
            for (const auto& slaveId : slaves)
            {
                heartbeat->addSlave(sim.simComponents()[slaveId]); 
            }
        }
    }
}
