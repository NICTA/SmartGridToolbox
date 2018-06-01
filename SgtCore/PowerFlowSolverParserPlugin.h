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

#ifndef POWER_FLOW_SOLVER_PARSER_PLUGIN_DOT_H
#define POWER_FLOW_SOLVER_PARSER_PLUGIN_DOT_H

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    class Network;

    /// @addtogroup NetworkYamlSpec
    /// @{
    /// YAML schema for `power_flow_solver` keyword.
    ///
    /// The `power_flow_solver` keyword sets the solver for a Network.
    ///
    /// ```
    /// - power_flow_solver:    <string>    # nr_rect for AC Newton-Raphson, or opf_pt for PowerTools-based AC OPF.
    /// ```
    /// @}

    /// @brief Parses the `power_flow_solver` keyword, setting the network's solver.
    /// @ingroup NetworkParserPlugins
    class PowerFlowSolverParserPlugin : public NetworkParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "power_flow_solver";
        }

        virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;
    };
}

#endif // POWER_FLOW_SOLVER_PARSER_PLUGIN_DOT_H
