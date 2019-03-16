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

#ifndef BATTERY_PARSER_PLUGIN_DOT_H
#define BATTERY_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimParser.h>

namespace Sgt
{
    class Simulation;

    /// @addtogroup SimYamlSpec
    /// @{
    /// <b>YAML schema for `battery` keyword.</b>
    ///
    /// The `battery` keyword adds a Battery Component to the Network.
    ///
    /// ~~~{.yaml}
    /// - battery:
    ///     id:                     <string>    # Component ID.
    ///     inverter_id:            <string>    # Inverter to which battery is connected.
    ///     dt:                     <hh:mm:ss>  # Component will update every dt. 
    ///     init_soc:               <real>      # Initial state of charge, MWh.
    ///     max_soc:                <real>      # Maximum state of charge, MWh.
    ///     max_charge_power:       <real>      # Maximum charging power, MW.
    ///     max_discharge_power:    <real>      # Maximum discharge power, MW.
    ///     charge_efficiency:      <real>      # Charging efficiency; d/dt SOC = P_chg * eff.
    ///     discharge_efficiency:   <real>      # Discharge efficiency; d/dt SOC = -P_dis / eff.
    ///     requested_power:        <real>      # Requested output DC power for battery, MW.
    /// ~~~
    /// @}

    /// @brief Parses the `battery` keyword, adding a Battery to the simulation.
    /// @ingroup SimParserPlugins
    class BatteryParserPlugin : public SimParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "battery";
        }

        public:
        virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
}

#endif // BATTERY_PARSER_PLUGIN_DOT_H
