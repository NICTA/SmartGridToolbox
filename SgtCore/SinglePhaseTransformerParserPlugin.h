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

#ifndef SINGLE_PHASE_TRANSFORMER_PARSER_PLUGIN
#define SINGLE_PHASE_TRANSFORMER_PARSER_PLUGIN

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    class SinglePhaseTransformer;
    class Network;

    /// @addtogroup NetworkYamlSpec
    /// @{
    /// <b>YAML schema for `single_phase_transformer` keyword.</b>
    ///
    /// The `single_phase_transformer` keyword adds a new SinglePhaseTransformer to the Network.
    ///
    /// ~~~{.yaml}
    /// - single_phase_transformer:
    ///     id:                     <string>    # Unique id of component.
    ///     bus_0_id:               <string>    # ID of bus 0 of the branch.
    ///     bus_1_id:               <string>    # ID of bus 0 of the branch.
    ///     nom_V_ratio:            <complex>   # Nominal voltage ratio.
    ///     off_nom_ratio:          <complex>   # Off-nominal voltage ratio, Delta (prim.) / Y (sec.).
    ///     leakage_impedance:      <complex>   # Leakage impedance parameter.
    /// ~~~
    /// @}

    /// @brief Parses the `single_phase_transformer` keyword, adding a SinglePhaseTransformer to the network.
    /// @ingroup NetworkParserPlugins
    class SinglePhaseTransformerParserPlugin : public NetworkParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "single_phase_transformer";
        }

        virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;

        std::unique_ptr<SinglePhaseTransformer> parseSinglePhaseTransformer(const YAML::Node& nd,
                const ParserBase& parser) const;
    };
}

#endif // SINGLE_PHASE_TRANSFORMER_PARSER_PLUGIN
