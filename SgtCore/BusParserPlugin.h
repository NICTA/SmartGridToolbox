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

#ifndef BUS_PARSER_PLUGIN_DOT_H
#define BUS_PARSER_PLUGIN_DOT_H

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    class Bus;
    class Network;

    /// @addtogroup NetworkYamlSpec
    /// @{
    /// <b>YAML schema for `bus` keyword.</b>
    ///
    /// The `bus` keyword adds a new Bus to the Network.
    ///
    /// ~~~{.yaml}
    /// - bus:
    ///     id:     <string>    # Unique id of component.
    ///     phases: <phases>    # Phases, e.g. [A, B, C]
    ///     type:   <bus_type>  # AC power flow type: SL/PV/PQ
    ///     V_base: <real>      # Base voltage e.g. 11
    ///     V_nom:  <real>      # Nominal voltage e.g. [11, 11D120, 11D-120]
    ///     coords: <real_vec>  # (Optional) Physical coordinates, [x, y], e.g. [1000, 12787]
    /// ~~~
    /// @}

    /// @brief Parses the `bus` keyword, adding a Bus to the network.
    /// @ingroup NetworkParserPlugins
    class BusParserPlugin : public NetworkParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "bus";
        }

        virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;

        std::unique_ptr<Bus> parseBus(const YAML::Node& nd, const ParserBase& parser) const;
    };
}

#endif // BUS_PARSER_PLUGIN_DOT_H
