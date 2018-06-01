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

#ifndef GEN_PARSER_PLUGIN_DOT_H
#define GEN_PARSER_PLUGIN_DOT_H

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    class Gen;
    class Network;

    /// @addtogroup NetworkYamlSpec
    /// @{
    /// YAML schema for `gen` keyword.
    ///
    /// The `gen` keyword adds a new Gen to the Network.
    ///
    /// ```
    /// - gen:
    ///     id: <string>        # Unique id of component.
    ///     bus_id: <string>    # Bus ID.
    ///     phases: <phases>    # Connection phases at bus e.g. [A, B, C]
    ///     S: <complex_vec>    # Complex power setpoint.
    ///     P_min: <real>       # Minimum P setpoint.
    ///     P_max: <real>       # Maximum P setpoint.
    ///     Q_min: <real>       # Minimum Q setpoint.
    ///     Q_max: <real>       # Maximum Q setpoint.
    ///     J: <real>           # Angular momentum.
    ///     C0: <real>          # Constant cost coefficient.
    ///     C1: <real>          # Linear cost coefficient.
    ///     C2: <real>          # Quadratic cost coefficient.
    /// ```
    /// @}

    /// @brief Parses the `gen` keyword, adding a Gen to the network.
    /// @ingroup NetworkParserPlugins
    class GenParserPlugin : public NetworkParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "gen";
        }

        virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;

        std::unique_ptr<Gen> parseGen(const YAML::Node& nd, const ParserBase& parser) const;
    };
}

#endif // GEN_PARSER_PLUGIN_DOT_H
