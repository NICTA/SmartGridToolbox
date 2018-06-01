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

#ifndef COMMON_BRANCH_PARSER_PLUGIN_DOT_H
#define COMMON_BRANCH_PARSER_PLUGIN_DOT_H

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    class CommonBranch;
    class Network;

    /// @addtogroup NetworkYamlSpec
    /// @{
    /// YAML schema for `common_branch` keyword.
    ///
    /// The `common_branch` keyword adds a new CommonBranch to the Network.
    ///
    /// ```
    /// - common_branch:
    ///     id:                 <string>    # Unique id of component.
    ///     bus_0_id:           <string>    # ID of bus 0 of the branch.
    ///     bus_1_id:           <string>    # ID of bus 0 of the branch.
    ///     Y_series:           <complex>   # Complex series admittance parameter.
    ///     Y_shunt:            <complex>   # Complex shunt admittance parameter.
    ///     complex_tap_ratio:  <complex>   # Complex tap ratio parameter.
    ///     rate_A:             <real>      # Line limit A.
    ///     rate_B:             <real>      # Line limit B.
    ///     rate_C:             <real>      # Line limit C.
    /// ```
    /// @}

    /// @brief Parses the `common_branch` keyword, adding a CommonBranch to the network.
    /// @ingroup NetworkParserPlugins
    class CommonBranchParserPlugin : public NetworkParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "common_branch";
        }

        virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;

        std::unique_ptr<CommonBranch> parseCommonBranch(const YAML::Node& nd, const ParserBase& parser) const;
    };
}

#endif // COMMON_BRANCH_PARSER_PLUGIN_DOT_H
