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

#ifndef ZIP_PARSER_PLUGIN_DOT_H
#define ZIP_PARSER_PLUGIN_DOT_H

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    class Network;
    class Zip;

    /// @addtogroup NetworkYamlSpec
    /// @{
    /// <b>YAML schema for `zip` keyword.</b>
    ///
    /// The `zip` keyword adds a new Zip to the Network.
    ///
    /// ~~~{.yaml}
    /// - zip:
    ///     id:         <string>        # Unique id of component.
    ///     bus_id:     <string>        # Bus ID.
    ///     phases:     <phases>        # Connection phases at bus e.g. [A, B, C]
    ///     Y_const:    <complex_mat>   # Upper triangular matrix of constant Y ZIP components.
    ///     I_const:    <complex_mat>   # Upper triangular matrix of constant I ZIP components.
    ///     S_const:    <complex_mat>   # Upper triangular matrix of constant S ZIP components.
    /// ~~~
    /// @}

    /// @brief Parses the `zip` keyword, adding a Zip to the network.
    /// @ingroup NetworkParserPlugins
    class ZipParserPlugin : public NetworkParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "zip";
        }

        virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;

        std::unique_ptr<Zip> parseZip(const YAML::Node& nd, const ParserBase& parser) const;
    };
}

#endif // ZIP_PARSER_PLUGIN_DOT_H
