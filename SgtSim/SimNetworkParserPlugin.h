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

#ifndef SIM_NETWORK_PARSER_PLUGIN_DOT_H
#define SIM_NETWORK_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimNetwork.h>
#include <SgtSim/SimParser.h>

namespace Sgt
{
    class Simulation;

    /// @addtogroup SimYamlSpec
    /// @{
    /// <b>YAML schema for `sim_network` keyword.</b>
    ///
    /// The `sim_network` keyword adds a new SimNetwork Component to the Simulation.
    ///
    /// ~~~{.yaml}
    /// - sim_network:
    ///     id: <string> # Component ID.
    ///     P_base: <real> # Base power, MW.
    ///     freq_Hz: <real> # Nominal frequency in Hz.
    ///     solver: <string> # Assigned network solver: currently "nr" (Newton-Raphson) or "opf_pt" (PowerTools OPF).
    ///     network: <string> # Complete network specification, see below. 
    /// ~~~
    ///
    /// Please see @ref NetworkYamlSpec for documentation about how to fill in the "network" keyword above.
    /// @}

    /// @brief Parses the `sim_network` keyword, adding a SimNetwork to the simulation.
    /// @ingroup SimParserPlugins
    class SimNetworkParserPlugin : public SimParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "sim_network";
        }

        public:
        virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
}

#endif // SIM_NETWORK_PARSER_PLUGIN_DOT_H
