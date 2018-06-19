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

#ifndef WEATHER_PARSER_PLUGIN_DOT_H
#define WEATHER_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimParser.h>

namespace Sgt
{
    class Simulation;

    /// @addtogroup SimYamlSpec
    /// @{
    /// <b>YAML schema for `weather` keyword.</b>
    ///
    /// The `weather` keyword adds a Weather Component to the Simulation.
    ///
    /// ~~~{.yaml}
    /// # TODO
    /// ~~~
    /// @}

    /// @brief Parses the weather keyword, adding a Weather component to the simulation.
    /// @ingroup SimParserPlugins
    class WeatherParserPlugin : public SimParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "weather";
        }

        public:
        virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
}

#endif // WEATHER_PARSER_PLUGIN_DOT_H
