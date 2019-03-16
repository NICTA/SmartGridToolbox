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

#ifndef MATPOWER_PARSER_PLUGIN_DOT_H
#define MATPOWER_PARSER_PLUGIN_DOT_H

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    class Network;

    /// @addtogroup NetworkYamlSpec
    /// @{
    /// <b>YAML schema for `matpower` keyword.</b>
    ///
    /// The `matpower` keyword adds a network from a matpower file to a network.
    ///
    /// ~~~{.yaml}
    /// - matpower:
    ///     default_kV_base:    <real>                      # Base voltage, kV, when none is given in matpower network.
    ///     # EITHER
    ///     input_file:         <string>                    # Name of matpower filename,
    ///     # OR
    ///     embedded_input:     <string>                    # Actual matpower input, use a YAML block/folded style.
    ///     scale_V_by:         <real>                      # Scaling factor for voltages.
    ///     scale_P_by:         <real>                      # Scaling factor for powers.
    ///     bus_names:          [<string>, <string>, ...]   # IDs for created buses, in matpower order.
    /// ~~~
    /// @}

    /// @brief Parses the `matpower` keyword, reading a matpower file into the network.
    /// @ingroup NetworkParserPlugins
    class MatpowerParserPlugin : public NetworkParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "matpower";
        }

        virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;
    };
}

#endif // MATPOWER_PARSER_PLUGIN_DOT_H
