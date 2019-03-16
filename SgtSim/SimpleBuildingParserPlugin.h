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

#ifndef SIMPLE_BUILDING_PARSER_PLUGIN_DOT_H
#define SIMPLE_BUILDING_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimParser.h>

namespace Sgt
{
    class SimpleBuilding;
    class Simulation;

    /// @addtogroup SimYamlSpec
    /// @{
    /// <b>YAML schema for `simple_building` keyword.</b>
    ///
    /// The `simple_building` keyword adds a new SimpleBuilding Component to the Simulation.
    ///
    /// ~~~{.yaml}
    /// - simple_building:
    ///     id:                     <string>    # Component ID.
    ///     bus_id:                 <string>    # ID of bus to which building is attached.
    ///     zip_id:                 <string>    # ID of network ZIP corresponding to the building. Defaults to id.
    ///     sim_network_id:         <string>    # ID of the SimNetwork.
    ///     dt:                     <hh:mm:ss>  # Update frequency.
    ///     kb:                     <real>      # Lumped thermal conductivity, W/K.
    ///     cb:                     <real>      # Heat capacity of building, J/K
    ///     Tb_init:                <real>      # Initial temperature of building.
    ///     kh:                     <real>      # HVAC PID parameter, W/K.
    ///     COP_cool:               <real>      # Coefficient of performance, cooling.
    ///     COP_heat:               <real>      # Coefficient of performance, heating.
    ///     P_max:                  <real>      # HVAC maximum power, W. 
    ///     Ts:                     <real>      # Temperature setpoint, °C.
    ///     internal_heat_power:    <string>    # ID of TimeSeries -> heat (W) gen. by occupants, appliances etc.
    ///     weather:                <string>    # ID of weather component.
    /// ~~~
    /// @}

    /// @brief Parses the `simple_building` keyword, adding a SimpleBuilding to the simulation.
    /// @ingroup SimParserPlugins
    class SimpleBuildingParserPlugin : public SimParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "simple_building";
        }

        public:
        virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
}

#endif // SIMPLE_BUILDING_PARSER_PLUGIN_DOT_H
