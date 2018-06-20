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

#ifndef TIME_SERIES_ZIP_PARSER_PLUGIN_DOT_H
#define TIME_SERIES_ZIP_PARSER_PLUGIN_DOT_H

#include <SgtSim/SimParser.h>

namespace Sgt
{
    class Simulation;

    /// @addtogroup SimYamlSpec
    /// @{
    /// <b>YAML schema for `time_series_zip` keyword.</b>
    ///
    /// The `time_series_zip` keyword adds a TimeSeriesZip to the Simulation.
    ///
    /// ~~~{.yaml}
    /// - time_series_zip:
    ///     id:                     <string>        # Component ID.
    ///     bus_id:                 <string>        # Attached to this bus.
    ///     sim_network_id:         <string>        # Part of this SimNetwork.
    ///     time_series_id:         <string>        # Use this TimeSeries for values.
    ///     dt:                     <hh:mm:ss>      # Update frequency.
    ///     matrix_elements:        <int_mat>       # n x 2 matrix corresponding to matrix elems of target ZIP matrix.
    ///     # Any combination of the following three elements:
    ///     data_indices_Y:         <int_vec>       # Data elements that will template onto ZIP matrix for const Y.
    ///     data_indices_I:         <int_vec>       # Data elements that will template onto ZIP matrix for const I.
    ///     data_indices_S:         <int_vec>       # Data elements that will template onto ZIP matrix for const S.
    ///     zip_id:                 <string>        # ID of Network Zip. If absent, will create a new zip.
    ///     phases:                 <phases >       # Phases of new ZIP, see above.
    ///     scale_factor_Y:         <real >         # Scale factor applied to constant Y.
    ///     scale_factor_I:         <real >         # Scale factor applied to constant I.
    ///     scale_factor_S:         <real >         # Scale factor applied to constant S.
    /// ~~~
    /// @}

    /// @brief Parses the time_series_zip keyword, adding a TimeSeriesZip to a simulation.
    /// @ingroup SimParserPlugins
    class TimeSeriesZipParserPlugin : public SimParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "time_series_zip";
        }

        public:
        virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
}

#endif // TIME_SERIES_ZIP_PARSER_PLUGIN_DOT_H
