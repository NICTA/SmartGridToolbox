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

#ifndef TIME_SERIES_TAP_CHANGER_PARSER_PLUGIN_DOT_H
#define TIME_SERIES_TAP_CHANGER_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimParser.h>

namespace Sgt
{
    class Simulation;

    /// @addtogroup SimYamlSpec
    /// @{
    /// <b>YAML schema for `time_series_tap_changer` keyword.</b>
    ///
    /// The `time_series_tap_changer` keyword adds a TimeSeriesTapChanger to the Simulation.
    ///
    /// ~~~{.yaml}
    /// - time_series_tap_changer:
    ///     id:                     <string>        # Component ID.
    ///     sim_network_id:         <string>        # SimNetwork ID.
    ///     transformer_id:         <string>        # ID of transformer to which I'm attached.
    ///     min_tap_ratio:          <real>          # Minimum tap ratio.
    ///     max_tap_ratio:          <real>          # Maximum tap ratio.
    ///     min_tap:                <int>           # Minimum tap setting.
    ///     n_taps:                 <int>           # Number of taps.
    ///     time_series_id:         <string>        # Time series ID.
    ///     dt:                     <duration>      # Update period.
    /// ~~~
    /// @}

    /// @brief Parses the `tap_changer` keyword, adding a TimeSeriesTapChanger to the simulation.
    /// @ingroup SimParserPlugins
    class TimeSeriesTapChangerParserPlugin : public SimParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "time_series_tap_changer";
        }

        public:
        virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
}

#endif // TIME_SERIES_TAP_CHANGER_PARSER_PLUGIN_DOT_H
