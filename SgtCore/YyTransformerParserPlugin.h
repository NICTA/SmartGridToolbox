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

#ifndef YY_TRANSFORMER_PARSER_PLUGIN
#define YY_TRANSFORMER_PARSER_PLUGIN

#include <SgtCore/NetworkParser.h>

namespace Sgt
{
    class YyTransformer;
    class Network;

    /// @brief Parses the `yy_transformer` keyword, adding a YyTransformer to the network.
    /// @ingroup NetworkParserPlugins
    class YyTransformerParserPlugin : public NetworkParserPlugin
    {
        public:
        virtual const char* key() const override
        {
            return "yy_transformer";
        }

        virtual void parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const override;

        std::unique_ptr<YyTransformer> parseYyTransformer(const YAML::Node& nd, const ParserBase& parser) const;
    };
}

#endif // YY_TRANSFORMER_PARSER_PLUGIN
