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

#ifndef DD_TRANSFORMER_PARSER_PLUGIN
#define DD_TRANSFORMER_PARSER_PLUGIN

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    class DdTransformer;
    class Network;

    /// @addtogroup NetworkYamlSpec
    /// @{
    /// <b>YAML schema for `dd_transformer` keyword.</b>
    ///
    /// The `dd_transformer` keyword adds a new DdTransformer to the Network.
    ///
    /// ~~~{.yaml}
    /// - dd_transformer:
    ///     id:                     <string>    # Unique id of component.
    ///     bus_0_id:               <string>    # ID of bus 0 of the branch.
    ///     bus_1_id:               <string>    # ID of bus 0 of the branch.
    ///     phases_0:               <string>    # Phases of connection to bus_0.
    ///     phases_1:               <string>    # Phases of connection to bus_1.
    ///     complex_turns_ratio_01: <complex>   # Complex turns ratio, side_0 / side_1.
    ///     leakage_impedance:      <complex>   # Leakage impedance parameter.
    ///     magnetizing_admittance: <complex>   # Magnetizing admittance parameter.
    /// ~~~
    /// @}

    /// @brief Parses the `dd_transformer` keyword, adding a DdTransformer to the network.
    /// @ingroup NetworkParserPlugins
    class DdTransformerParserPlugin : public NetworkParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "dd_transformer";
        }

        virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;

        std::unique_ptr<DdTransformer> parseDdTransformer(const YAML::Node& nd, const ParserBase& parser) const;
    };
}

#endif // DD_TRANSFORMER_PARSER_PLUGIN
