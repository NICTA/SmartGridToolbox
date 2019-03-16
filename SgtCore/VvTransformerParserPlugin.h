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

#ifndef VV_TRANSFORMER_PARSER_PLUGIN
#define VV_TRANSFORMER_PARSER_PLUGIN

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    class VvTransformer;
    class Network;

    /// @addtogroup NetworkYamlSpec
    /// @{
    /// <b>YAML schema for `vv_transformer` keyword.</b>
    ///
    /// The `vv_transformer` keyword adds a new (open-delta) VvTransformer to the Network.
    ///
    /// ~~~{.yaml}
    /// - vv_transformer:
    ///     id:                     <string>    # Unique id of component.
    ///     bus_0_id:               <string>    # ID of bus 0 of the branch.
    ///     bus_1_id:               <string>    # ID of bus 0 of the branch.
    ///     phases_0:               <string>    # Phases of connection to bus_0.
    ///     phases_1:               <string>    # Phases of connection to bus_1.
    ///     nom_ratio_w1:           <complex>   # Nominal voltage ratio, first winding pair.
    ///     nom_ratio_w2:           <complex>   # Nominal voltage ratio, second winding pair.
    ///     off_nom_ratio_w1:       <complex>   # Off-nominal voltage ratio, first winding pair.
    ///     off_nom_ratio_w2:       <complex>   # Off-nominal voltage ratio, second winding pair.
    ///     leakage_impedance_w1:   <complex>   # Leakage impedance parameter, first winding pair.
    ///     leakage_impedance_w2:   <complex>   # Leakage impedance parameter, second winding pair.
    ///     # Either option 1:
    ///     tie_admittance:         <complex>   # Admittance tying middle phases together.
    ///     # Or option 2:
    ///     ground_admittance:      <complex>   # Admittance tying middle phase of secondary to ground.
    /// ~~~
    /// @}

    /// @brief Parses the `vv_transformer` keyword, adding a VvTransformer to the network.
    /// @ingroup NetworkParserPlugins
    class VvTransformerParserPlugin : public NetworkParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "vv_transformer";
        }

        virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;

        std::unique_ptr<VvTransformer> parseVvTransformer(const YAML::Node& nd, const ParserBase& parser) const;
    };
}

#endif // VV_TRANSFORMER_PARSER_PLUGIN
