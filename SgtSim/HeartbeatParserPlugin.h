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

#ifndef HEARTBEAT_PARSER_PLUGIN_DOT_H
#define HEARTBEAT_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimParser.h>

namespace Sgt
{
    class Simulation;

    /// @addtogroup SimYamlSpec
    /// @{
    /// <b>YAML schema for `heartbeat` keyword.</b>
    ///
    /// The `heartbeat` keyword adds a Heartbeat Component to the Network.
    ///
    /// ~~~{.yaml}
    /// - heartbeat:
    ///     id:                     <string>                    # Component ID.
    ///     dt:                     <hh:mm:ss>                  # Component will update every dt.
    ///     slaves:                 [<string>, <string>, ...]   # Optional Components to update when I update.
    /// ~~~
    /// @}

    /// @brief Parses the `heartbeat` keyword, adding a Heartbeat to the simulation.
    /// @ingroup SimParserPlugins
    class HeartbeatParserPlugin : public SimParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "heartbeat";
        }

        public:
        virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
}

#endif // HEARTBEAT_PARSER_PLUGIN_DOT_H
