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

#ifndef PARSER_DOT_H
#define PARSER_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/YamlSupport.h>

#include <yaml-cpp/yaml.h>

#include <map>
#include <regex>

namespace Sgt
{
    void assertFieldPresent(const YAML::Node& nd, const std::string& field);

    YAML::Node getTopNode(const std::string& fname);

    template<typename T> class Parser;

    // Some no-templated functionality in the base class.
    class ParserBase
    {
        public:

            template<typename T> T expand(const YAML::Node& nd) const
            {
                return YAML::Load(expandString(nd2Str(nd))).as<T>();
            }

            template<typename T> Parser<T> subParser() const;

        protected:

            struct ParserLoop
            {
                std::string name_;
                int i_; // The number associated with the current iteration, starts are init and increments by stride.
                int upper_;
                int stride_;
                YAML::Node body_;
            };

        protected:

            ParserLoop& parseLoop(const YAML::Node& nd);

        private:

            std::string expandString(const std::string& str) const;
            std::string expandExpression(const std::smatch& m) const;
            std::string expandMathExpressionBody(const std::string& str) const;
            std::string expandLoopOrParameterExpressionBody(const std::string& str1, const std::string& str2) const;
            std::string nd2Str(const YAML::Node& nd) const;

        protected:

            std::map<std::string, YAML::Node> parameters_;
            std::vector<ParserLoop> loops_;
    };

    /// @brief A plugin to parse YAML into an object.
    /// @ingroup Parsing
    template<typename T> class ParserPlugin
    {
        public:
            virtual ~ParserPlugin() = default;
            virtual const char* key() {return "ERROR";}
            virtual void parse(const YAML::Node& nd, T& into, const ParserBase& parser) const
            {
                // Empty.
            }
            virtual void postParse(const YAML::Node& nd, T& into, const ParserBase& parser) const
            {
                // Empty.
            }
    };

    template<typename T> class Parser;
    template<typename T> void registerParserPlugins(Parser<T>& parser);

    /// @brief A Parser to parse YAML into an object.
    /// @ingroup Parsing
    template<typename T> class Parser : public ParserBase
    {
        public:
            Parser()
            {
                registerParserPlugins(*this);
            }

            template<typename PluginType> void registerParserPlugin()
            {
                auto plugin = std::unique_ptr<PluginType>(new PluginType());
                plugins_[plugin->key()] = std::move(plugin);
            }

            void parse(const std::string& fname, T& into)
            {
                Log().message() << "Parsing file " << fname << "." << std::endl;
                {
                    LogIndent _;
                    auto top = getTopNode(fname);
                    parse(top, into);
                }
                Log().message() << "Finished parsing file " << fname << "." << std::endl;
            }

            void parse(const YAML::Node& node, T& into)
            {
                for (const auto& subnode1 : node)
                {
                    for (const auto& subnode : subnode1)
                    {
                        // Normally, there will be only one map per list item, but it is OK to have more.
                        // The issue is that maps have no implied ordering.
                        std::string nodeType = subnode.first.as<std::string>();
                        const YAML::Node& nodeVal = subnode.second;

                        if (nodeType == "parameters")
                        {
                            for (auto& nd : nodeVal)
                            {
                                std::string key = nd.first.as<std::string>();
                                auto val = nd.second;
                                parameters_[key] = val;
                            }
                        }
                        else if (nodeType == "loop")
                        {
                            for (auto& l = parseLoop(nodeVal); l.i_ < l.upper_; l.i_ += l.stride_)
                            {
                                SGT_DEBUG(
                                    Log().message() << "LOOP: " << l.name_ << " : " << l.i_ << std::endl; LogIndent _);
                                parse(l.body_, into);
                            }
                            loops_.pop_back();
                        }
                        else
                        {
                            SGT_DEBUG(Log().message() << "Parsing plugin " <<  nodeType << "." << std::endl);
                            auto it = plugins_.find(nodeType);
                            if (it == plugins_.end())
                            {
                                Log().warning() << "I don't know how to parse plugin " << nodeType << std::endl;
                            }
                            else
                            {
                                SGT_DEBUG(LogIndent _);
                                it->second->parse(nodeVal, into, *this);
                            }
                            SGT_DEBUG(Log().message() << "Finished parsing plugin " <<  nodeType << "." << std::endl);
                        }
                    }
                }
            }

        private:

            std::map<std::string, std::unique_ptr<ParserPlugin<T>>> plugins_;
    };
            
    template<typename T> Parser<T> ParserBase::subParser() const
    {
        Parser<T> result;
        result.parameters_ = parameters_;
        result.loops_ = loops_;
        return result;
    }
}

#endif // PARSER_DOT_H
