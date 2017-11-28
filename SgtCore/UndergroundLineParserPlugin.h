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

#ifndef UNDERGROUND_LINE_PARSER_PLUGIN
#define UNDERGROUND_LINE_PARSER_PLUGIN

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    class Network;
    class UndergroundLine;

    /// @brief ParserPlugin that parses UndergroundLine objects.
    /// @ingroup Parsing
    class UndergroundLineParserPlugin : public NetworkParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "underground_line";
        }

        virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;

        std::unique_ptr<UndergroundLine> parseUndergroundLine(const YAML::Node& nd, const ParserBase& parser) const;
    };
}

#endif // UNDERGROUND_LINE_PARSER_PLUGIN
