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

#ifndef INVERTER_ZIP_PARSER_PLUGIN_DOT_H
#define INVERTER_ZIP_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimParser.h>

namespace Sgt
{
    class Simulation;

    /// @addtogroup SimYamlSpec
    /// @{
    /// <b>YAML schema for `inverter` keyword.</b>
    ///
    /// The `inverter` keyword adds an Inverter Component to the Network.
    ///
    /// ~~~{.yaml}
    /// - inverter:
    ///     id:                     <string>    # Component ID.
    ///     sim_network_id:         <string>    # SimNetwork to which inverter belongs.
    ///     bus_id:                 <string>    # Bus to which inverter is attached.
    ///     phases:                 <phases>    # Bus Phases, e.g. [A, B, C] to which power is produced.
    ///     is_delta:               <bool>      # If true, use a Delta connection.
    ///     efficiency_dc_to_ac:    <real>      # Efficiency when injecting grid power.
    ///     efficiency_ac_to_dc:    <real>      # Efficiency when consuming grid power.
    ///     max_S_mag:              <real>      # Max apparent power |S|, MVA.
    ///     requested_Q:            <real>      # Requested reactive power, MVAr. May curtail for max_S_mag. 
    /// ~~~
    /// @}

    /// @brief Parses the `inverter` keyword, adding an Inverter to the simulation.
    /// @ingroup SimParserPlugins
    class InverterParserPlugin : public SimParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "inverter";
        }

        public:
        virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
}

#endif // INVERTER_ZIP_PARSER_PLUGIN_DOT_H
