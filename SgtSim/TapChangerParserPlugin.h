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

#ifndef TAP_CHANGER_PARSER_PLUGIN_DOT_H
#define TAP_CHANGER_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimParser.h>

namespace Sgt
{
    class Simulation;

    /// @addtogroup SimYamlSpec
    /// @{
    /// <b>YAML schema for `tap_changer` keyword.</b>
    ///
    /// The `tap_changer` keyword adds a TapChanger to the Simulation.
    ///
    /// ~~~{.yaml}
    /// # TODO
    /// ~~~
    /// @}

    /// @brief Parses the `tap_changer` keyword, adding a TapChanger to the simulation.
    /// @ingroup SimParserPlugins
    class TapChangerParserPlugin : public SimParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "tap_changer";
        }

        public:
        virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
}

#endif // TAP_CHANGER_PARSER_PLUGIN_DOT_H
