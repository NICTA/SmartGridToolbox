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

#ifndef DGY_TRANSFORMER_PARSER_PLUGIN
#define DGY_TRANSFORMER_PARSER_PLUGIN

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    class DgyTransformer;
    class Network;

    /// @addtogroup NetworkYamlSpec
    /// @{
    /// <b>YAML schema for `dgy_transformer` keyword.</b>
    ///
    /// The `dgy_transformer` keyword adds a new DgyTransformer to the Network.
    ///
    /// ~~~{.yaml}
    /// - dgy_transformer:
    ///     id:                     <string>    # Unique id of component.
    ///     bus_0_id:               <string>    # ID of bus 0 of the branch.
    ///     bus_1_id:               <string>    # ID of bus 0 of the branch.
    ///     phases_0:               <string>    # Phases of connection to bus_0.
    ///     phases_1:               <string>    # Phases of connection to bus_1.
    ///     nom_V_ratio_DY:         <complex>   # Nominal voltage ratio, Delta (prim.) / Y (sec.).
    ///     off_nom_ratio_DY:       <complex>   # Off-nominal voltage ratio, Delta (prim.) / Y (sec.).
    ///     leakage_impedance:      <complex>   # Leakage impedance parameter.
    /// ~~~
    /// @}

    /// @brief Parses the `dgy_transformer` keyword, adding a DgyTransformer to the network.
    /// @ingroup NetworkParserPlugins
    class DgyTransformerParserPlugin : public NetworkParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "dgy_transformer";
        }

        virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;

        std::unique_ptr<DgyTransformer> parseDgyTransformer(const YAML::Node& nd, const ParserBase& parser) const;
    };
}

#endif // DGY_TRANSFORMER_PARSER_PLUGIN
