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

#include "TapChangerParserPlugin.h"

#include "Inverter.h"
#include "Simulation.h"
#include "TapChanger.h"
#include "Weather.h"

namespace Sgt
{
    void TapChangerParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "network_id");
        assertFieldPresent(nd, "bus_id");
        assertFieldPresent(nd, "target_id");
        assertFieldPresent(nd, "taps");
        assertFieldPresent(nd, "setpoint");
        assertFieldPresent(nd, "tolerance");
        assertFieldPresent(nd, "get_set_property");

        std::string id = parser.expand<std::string>(nd["id"]);
        std::string simNetworkId = parser.expand<std::string>(nd["sim_network_id"]);
        std::string busId = parser.expand<std::string>(nd["bus_id"]);
        std::string targetId = parser.expand<std::string>(nd["target_id"]);

        std::vector<double> taps = parser.expand<std::vector<double>>(nd["taps"]);
        double setpoint = parser.expand<double>(nd["setpoint"]);
        double tolerance = parser.expand<double>(nd["tolerance"]);
        std::string propName = parser.expand<std::string>(nd["get_set_property"]);

        ConstSimComponentPtr<SimNetwork> simNetwork = sim.simComponent<SimNetwork>(simNetworkId);
        ConstComponentPtr<Bus> bus = simNetwork->network().buses()[busId];
        ComponentPtr<BranchAbc> target = simNetwork->network().branches()[targetId];
        const Property<double, double>& prop = 
            dynamic_cast<const Property<double, double>&>(target->properties()[propName]);

        sim.newSimComponent<TapChanger>(id, bus, taps, setpoint, tolerance, prop, target);
    }
}
