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
        assertFieldPresent(nd, "sim_network_id");
        assertFieldPresent(nd, "control_bus_id");
        assertFieldPresent(nd, "target_id");

        assertFieldPresent(nd, "taps");
        assertFieldPresent(nd, "setpoint");
        assertFieldPresent(nd, "tolerance");

        assertFieldPresent(nd, "control_bus_getter");
        assertFieldPresent(nd, "target_setter");

        std::string id = parser.expand<std::string>(nd["id"]);
        std::string simNetworkId = parser.expand<std::string>(nd["sim_network_id"]);
        std::string controlBusId = parser.expand<std::string>(nd["control_bus_id"]);
        std::string targetId = parser.expand<std::string>(nd["target_id"]);

        std::vector<double> taps = parser.expand<std::vector<double>>(nd["taps"]);
        double setpoint = parser.expand<double>(nd["setpoint"]);
        double tolerance = parser.expand<double>(nd["tolerance"]);

        std::string controlBusGetterId = parser.expand<std::string>(nd["control_bus_getter"]);
        std::string targetSetterId = parser.expand<std::string>(nd["target_setter"]);

        ConstSimComponentPtr<SimNetwork> simNetwork = sim.simComponent<SimNetwork>(simNetworkId);
        ConstComponentPtr<Bus> controlBus = simNetwork->network().buses()[controlBusId];
        ComponentPtr<BranchAbc> target = simNetwork->network().branches()[targetId];
        const Getter<double>& controlBusGetter = 
            dynamic_cast<const Getter<double>&>(*target->properties()[controlBusGetterId].getter());
        
        const Setter<double>& targetSetter = 
            dynamic_cast<const Setter<double>&>(*target->properties()[targetSetterId].setter());

        sim.newSimComponent<TapChanger>(id, taps, setpoint, tolerance,
                controlBus, target, controlBusGetter, targetSetter);
    }
}
