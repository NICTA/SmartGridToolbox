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
        auto ndTargetSetter = nd["target_setter"];
        assertFieldPresent(ndTargetSetter, "id");
        assertFieldPresent(ndTargetSetter, "type");
        std::string targetSetterId = parser.expand<std::string>(ndTargetSetter["id"]);
        std::string targetSetterType = parser.expand<std::string>(ndTargetSetter["type"]);

        ConstSimComponentPtr<SimNetwork> simNetwork = sim.simComponent<SimNetwork>(simNetworkId);
        sgtAssert(simNetwork != nullptr, std::string(key()) + ": sim_network_id = " + simNetworkId + " was not found.");

        ConstComponentPtr<Bus> controlBus = simNetwork->network().buses()[controlBusId];
        sgtAssert(controlBus != nullptr, std::string(key()) + ": control_bus_id = " + controlBusId + " was not found.");

        ComponentPtr<BranchAbc> target = simNetwork->network().branches()[targetId];
        sgtAssert(target != nullptr, std::string(key()) + ": target_id = " + targetId + " was not found.");

        auto controlBusGetterFunc = TapChanger::get(controlBus, controlBusGetterId);

        const PropertyAbc* targetProp = nullptr;
        try
        {
            targetProp = &target->properties()[targetSetterId];
        }
        catch (std::out_of_range e)
        {
            sgtError(std::string(key()) + ": target_setter = " + targetSetterId + " was not found.");
        }
        sgtAssert(targetProp->setter() != nullptr, 
                std::string(key()) + ": target_setter = " + targetSetterId + " is not settable.");
        std::function<void (double)> targetSetterFunc;
        if (targetSetterType == "value")
        {
            auto setter = dynamic_cast<const Setter<double>*>(targetProp->setter());
            sgtAssert(setter != nullptr, 
                    std::string(key()) + ": target_setter = " + targetSetterId + " was of the wrong type.");
            targetSetterFunc = TapChanger::setVal(target, *setter);
        }
        else if (targetSetterType == "magnitude")
        {
            sgtAssert(targetProp->getter() != nullptr, 
                    std::string(key()) + ": target_setter = " + targetSetterId + " is not gettable.");
            auto prop = dynamic_cast<const Property<Complex, Complex>*>(targetProp);
            sgtAssert(prop != nullptr, 
                    std::string(key()) + ": target_setter = " + targetSetterId + " was of the wrong type.");
            targetSetterFunc = TapChanger::setMag(target, *prop);
        }

        sim.newSimComponent<TapChanger>(id, taps, setpoint, tolerance,
                controlBus, controlBusGetterFunc, targetSetterFunc);
    }
}
