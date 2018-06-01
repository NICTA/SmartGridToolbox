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

#ifndef OVERHEAD_LINE_PARSER_PLUGIN
#define OVERHEAD_LINE_PARSER_PLUGIN

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    class Network;
    class OverheadLine;

    /// @addtogroup NetworkYamlSpec
    /// @{
    /// YAML schema for `overhead_line` keyword.
    ///
    /// The `overhead_line` keyword adds an OverheadLine to the Network.
    ///
    /// ```
    /// - overhead_line:
    ///     id:                 <string>    # Unique id of component.
    ///     bus_0_id:           <string>    # ID of bus 0 of the branch.
    ///     bus_1_id:           <string>    # ID of bus 0 of the branch.
    ///     phases_0:           <phases>    # Phases of connection to bus_0.
    ///     phases_1:           <phases>    # Phases of connection to bus_1.
    ///     length:             <real>      # Line length, arbitrary units.
    ///     cond_dist:          <real_mat>  # Matrix of distances between each conductor.
    ///     subcond_gmr:        <real_vec>  # GMR of each conductor or subconductor if stranded.
    ///     subcond_r_per_l:    <real_vec>  # Ohms per unit length of each conductor or subconductor if stranded.
    ///     freq:               <real>      # Assumed frequency of operation.
    ///     earth_resist:       <real>      # Earth resistivity.
    ///     n_in_bundle:        <real>      # Optional number of subconductors in each bundle if stranded.
    ///     adj_subcond_dist:   <real>      # Distance between adjacent subconductors if stranded.
    /// ```
    /// @}

    /// @brief Parses the `overhead_line` keyword, adding an OverheadLine to the network.
    /// @ingroup NetworkParserPlugins
    class OverheadLineParserPlugin : public NetworkParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "overhead_line";
        }

        virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;

        std::unique_ptr<OverheadLine> parseOverheadLine(const YAML::Node& nd, const ParserBase& parser) const;
    };
}

#endif // OVERHEAD_LINE_PARSER_PLUGIN
