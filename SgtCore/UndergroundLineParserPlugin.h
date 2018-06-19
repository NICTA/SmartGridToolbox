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

#ifndef UNDERGROUND_LINE_PARSER_PLUGIN
#define UNDERGROUND_LINE_PARSER_PLUGIN

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    class Network;
    class UndergroundLine;

    /// @addtogroup NetworkYamlSpec
    /// @{
    /// <b>YAML schema for `underground_line` keyword.</b>
    ///
    /// The `underground_line` keyword adds a new UndergroundLine to the Network.
    ///
    /// ~~~{.yaml}
    /// - underground_line:
    ///     id:                     <string>        # Unique id of component.
    ///     bus_0_id:               <string>        # ID of bus 0 of the branch.
    ///     bus_1_id:               <string>        # ID of bus 0 of the branch.
    ///     phases_0:               <string>        # Phases of connection to bus_0.
    ///     phases_1:               <string>        # Phases of connection to bus_1.
    ///     length:                 <real>          # Line length, m.
    ///     has_neutral:            <bool>          # Is there a neutral wire?
    ///     shielding_type:         <string>        # "concentric_stranded" or "tape".
    ///     distance_matrix:        <real_matrix>   # Distance between wires.
    ///     gmr_phase:              <real>          # GMR of the phase wires.
    ///     gmr_neutral:            <real>          # GMR of the neutral wire (if present).
    ///     R_per_L_phase:          <real>          # Resistance per unit length, Ohms / m.
    ///     freq:                   <real>          # Nominal network frequency.
    ///     earth_resistivity:      <real>          # Earth resistivity. 
    ///     gmr_conc_strand:        <real>          # GMR of strands if concentric_stranded.
    ///     R_per_L_conc_strand:    <real>          # GMR of strands if concentric_stranded.
    ///     n_conc_strands:         <int>           # Number of strands if concentric_stranded.
    ///     radius_conc:            <int>           # Concentric radius if concentric_stranded.
    ///     outside_radius_shield:  <real>          # Outside radius if tape shielded.
    ///     thickness_shield:       <real>          # Shielding thickness if tape shielded.
    ///     resistivity_shield:     <real>          # Shielding resistivity if tape shielded.
    /// ~~~
    /// @}

    /// @brief Parses the `underground_line` keyword, adding an UndergroundLine to the network.
    /// @ingroup NetworkParserPlugins
    class UndergroundLineParserPlugin : public NetworkParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "underground_line";
        }

        virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;

        std::unique_ptr<UndergroundLine> parseUndergroundLine(const YAML::Node& nd, const ParserBase& parser) const;
    };
}

#endif // UNDERGROUND_LINE_PARSER_PLUGIN
