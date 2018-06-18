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

#ifndef BRANCH_PARSER_PLUGIN_DOT_H
#define BRANCH_PARSER_PLUGIN_DOT_H

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    class GenericBranch;
    class Network;

    /// @addtogroup NetworkYamlSpec
    /// @{
    /// <b>YAML schema for `generic_branch` keyword.</b>
    ///
    /// The `generic_branch` keyword adds a matpower-style GenericBranch to the Network.
    ///
    /// ~~~{.yaml}
    /// - generic_branch:
    ///     id:                     <string>        # Unique id of component.
    ///     bus_0_id:               <string>        # ID of bus 0 of the branch.
    ///     bus_1_id:               <string>        # ID of bus 0 of the branch.
    ///     phases_0:               <string>        # Phases of connection to bus_0.
    ///     phases_1:               <string>        # Phases of connection to bus_1.
    ///     Y:                                      # Alternative 1:
    ///         matrix:             <complex_mat>
    ///     Y:                                      # Alternative 2:
    ///         approximate_phase_impedance:         
    ///             Z+:             <real>          # +ve/-ve sequence impedance.
    ///             Z0:             <real>          # Zero sequence impedance.
    ///     impedance_multiplier:   <string>        # Optional multiplier for all admittances.
    /// ~~~
    /// @}

    /// @brief Parses the `generic_branch` keyword, adding a GenericBranch to the network.
    /// @ingroup NetworkParserPlugins
    class GenericBranchParserPlugin : public NetworkParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "generic_branch";
        }

        virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;

        std::unique_ptr<GenericBranch> parseGenericBranch(const YAML::Node& nd, const ParserBase& parser) const;
    };
}

#endif // BRANCH_PARSER_PLUGIN_DOT_H
