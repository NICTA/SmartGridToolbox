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

#ifndef TIME_SERIES_PARSER_PLUGIN_DOT_H
#define TIME_SERIES_PARSER_PLUGIN_DOT_H

#include <SgtSim/Simulation.h>
#include <SgtSim/SimParser.h>

namespace Sgt
{
    class Simulation;

    /// @addtogroup SimYamlSpec
    /// @{
    /// <b>YAML schema for `time_series` keyword.</b>
    ///
    /// The `time_series` keyword adds a TimeSeries to the Simulation.
    ///
    /// ~~~{.yaml}
    /// - time_series:
    ///     id:                     <string>                # TimeSeries ID.
    ///     type:                   <string>                # const_time_series/data_time_series
    ///     value_type:             <string>                # real_scalar/complex_scalar/real_vector/complex_vector
    ///     # If this is a const_time_series:
    ///     const_value:            <value_type>            # Constant value.
    ///     # If this is a data_time_series:
    ///     data_file:              <string>                # Filename of file containing t val [val ...] data.
    ///     interp_type:            <string>                # stepwise/lerp. 
    ///     time_unit:              <string>                # d/h/m/s.
    ///     relative_to_time:       <yyyy-mm-dd hh:mm:ss>   # Zero of time column corresponds to this time.
    ///     # If this is a vector time series:
    ///     dimension:              <int>                   # Number of dimensions of vector data.
    /// ~~~
    /// @}

    /// @brief Parses the time_series keyword, adding a TimeSeries to the simulation.
    /// @ingroup SimParserPlugins
    class TimeSeriesParserPlugin : public SimParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "time_series";
        }

        public:
        virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
}

#endif // TIME_SERIES_PARSER_PLUGIN_DOT_H
