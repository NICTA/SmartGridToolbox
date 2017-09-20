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

#include "NetworkParser.h"

#include "Common.h"
#include "PowerFlow.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

namespace
{
    using boost::spirit::ascii::space;

    template <typename Iterator> struct Calculator : qi::grammar<Iterator, int(), ascii::space_type>
    {
        Calculator() : Calculator::base_type(expression)
    {
        qi::_val_type _val;
        qi::_1_type _1;
        qi::uint_type uint_;

        expression =
            term                            [_val = _1]
            >> *(   ('+' >> term            [_val += _1])
                    |   ('-' >> term            [_val -= _1])
                )
            ;

        term =
            factor                          [_val = _1]
            >> *(   ('*' >> factor          [_val *= _1])
                    |   ('/' >> factor          [_val /= _1])
                    |   ('%' >> factor          [_val = _val % _1])
                )
            ;

        factor =
            uint_                           [_val = _1]
            |   '(' >> expression           [_val = _1] >> ')'
            |   ('-' >> factor              [_val = -_1])
            |   ('+' >> factor              [_val = _1])
            ;
    }

    qi::rule<Iterator, int(), ascii::space_type> expression, term, factor;
    };

    Calculator<std::string::const_iterator> calc;
}

namespace Sgt
{
    void assertFieldPresent(const YAML::Node& nd, const std::string& field)
    {
        sgtAssert(nd[field], "Parsing: " << field << " field not present.");
    };

    YAML::Node getTopNode(const std::string& fname)
    {
        sgtLogMessage() << "Opening file " << fname << " for parsing." << std::endl;
        auto top = YAML::LoadFile(fname);
        sgtAssert(top.size() > 0, "File " << fname << " is empty or doesn't exist.");
        return top;
    }

    ParserBase::ParserLoop& ParserBase::parseLoop(const YAML::Node& nd)
    {
        assertFieldPresent(nd, "loop_variable");
        assertFieldPresent(nd, "loop_body");

        const YAML::Node& ndLoopVar = nd["loop_variable"];
        const YAML::Node& ndBody = nd["loop_body"];

        std::string name = ndLoopVar[0].as<std::string>();
        sgtAssert(ndLoopVar.size() == 4, 
                "loop_variable expression is invalid. Format is [name, start, upper, stride].");
        int start = expand<int>(ndLoopVar[1]);
        int upper = expand<int>(ndLoopVar[2]);
        int stride = expand<int>(ndLoopVar[3]);
        loops_.push_back(std::unique_ptr<ParserLoop>(new ParserLoop{name, start, upper, stride, ndBody}));

        return *loops_.back();
    }
            
    void ParserBase::parseLogging(const YAML::Node& nd)
    {
        auto ndIndent = nd["indent"];
        if (ndIndent)
        {
            LogIndent::defaultTabWidth = ndIndent.as<unsigned int>();
        }

        for (const auto& subNd : nd)
        {
            if (subNd.IsScalar()) continue;

            std::string nodeType = subNd.first.as<std::string>();
            const YAML::Node& nodeVal = subNd.second;

            LogLevel* logLevel = nullptr;
            std::string* logFilter = nullptr; 
            if (nodeType == "message") 
            {
                logLevel = &messageLogLevel(); logFilter = &messageLogFilter();
            }
            else if (nodeType == "warning")
            {
                logLevel = &warningLogLevel(); logFilter = &warningLogFilter();
            }
            else if (nodeType == "error")
            {
                logLevel = &errorLogLevel(); logFilter = &errorLogFilter();
            }
            else if (nodeType == "debug")
            {
                logLevel = &debugLogLevel(); logFilter = &debugLogFilter();
            }

            const YAML::Node& ndLogLevel = nodeVal["log_level"];
            if (ndLogLevel)
            {
                auto lev = ndLogLevel.as<std::string>();
                if (lev == "none")
                {
                    *logLevel = LogLevel::NONE;
                }
                else if (lev == "normal")
                {
                    *logLevel = LogLevel::NORMAL;
                }
                else if (lev == "verbose")
                {
                    *logLevel = LogLevel::VERBOSE;
                }
                else
                {
                    sgtError(std::string("Unknown log level ") + lev);
                }
            }

            const YAML::Node& ndLogFilter = nodeVal["log_filter"];
            if (ndLogFilter)
            {
                *logFilter = ndLogFilter.as<std::string>();
            }
        }
    }

    namespace
    {
        const std::regex expr(                      // Submatch 0: whole expr, with <>.
            "<"
            "("                                     // Submatch 1: whole expr body, without <>.
                "("                                 // Submatch 2: whole math expr body.
                    "[-+*/%0-9.eEfFlL() \t]+"       // Any string of allowed chars in math expression.
                ")|"                                // End submatch 2.
                "("                                 // Submatch 3: whole var/loop expr body.
                    "("                             // Submatch 4: non-index part of var/loop expr body.
                        "[a-zA-Z_][a-zA-Z0-9_]*"    // Word not starting with digit.
                    ")+"                            // End submatch 4.
                    "("                             // Submatch 5: index part of var/loop expr body, with ().
                        "\\("
                        "("                         // Submatch 6: index part of var/loop expr, without ().
                            "[a-zA-Z0-9_]*"
                        ")"                         // End submatch 6.
                        "\\)"
                    ")?"                            // End submatch 5.
                ")"                                 // End submatch 3.
            ")"                                     // End submatch 1.
            ">"
        );                                          // End submatch 0.
    }

    std::string ParserBase::expandString(const std::string& str) const
    {
        sgtLogDebug(LogLevel::VERBOSE) << "Expand:" << std::endl;
        LogIndent indent;
        sgtLogDebug(LogLevel::VERBOSE) << "String:" << std::endl;
        indent.in(); sgtLogDebug(LogLevel::VERBOSE) << str << std::endl; indent.out();
        std::string result;

        // Iterator over expansion expressions in the target.
        std::sregex_iterator begin(str.cbegin(), str.cend(), expr);
        std::sregex_iterator end;

        if (begin == end)
        {
            // No expressions, just return the string.
            result = str;
        }
        else
        {
            std::string exprSuffix;
            for (auto it = begin; it != end; ++it) // Loop over all expansion expressions.
            {
                result += it->prefix();
                exprSuffix = it->suffix();
                std::string exprExpansion = expandExpression(*it);
                result += exprExpansion; // Put the prefix on the result.
            }
            result += exprSuffix;
            // Now recursively check if the expansion of this expression has created any more expressions to expand.
            result = expandString(result);
        }
        sgtLogDebug(LogLevel::VERBOSE) << "Result:" << std::endl;
        indent.in(); sgtLogDebug(LogLevel::VERBOSE) << result << std::endl; indent.out();
        return result;
    }

    std::string ParserBase::expandExpression(const std::smatch& m) const
    {
        std::string result;
        if (m[2] != "")
        {
            // Math expression, e.g. ${2 + 3/2}.
            result = expandMathExpressionBody(m[1]);
        }
        else
        {
            // Loop or parameter expression, e.g. $idx or $phases[3].
            result = expandLoopOrParameterExpressionBody(m[4], m[6]);
        }
        return result;
    }

    std::string ParserBase::expandMathExpressionBody(const std::string& str) const
    {
        int calcResult;
        std::string::const_iterator i1 = str.begin();
        std::string::const_iterator i2 = str.end();
        bool ok = phrase_parse(i1, i2, calc, space, calcResult); // And do the math...
        sgtAssert(ok, "Ill-formed expression " << str << ".");
        std::string result = std::to_string(calcResult);
        return result;
    }

    std::string ParserBase::expandLoopOrParameterExpressionBody(const std::string& s1, const std::string& s2) const
    {
        std::string result;

        auto it = std::find_if(loops_.begin(), loops_.end(), 
                [&s1](const std::unique_ptr<ParserLoop>& l) {return l->name_ == s1;});
        if (it != loops_.end())
        {
            // We matched a loop variable. Make sure there is no index!
            sgtAssert(s2 == "", "Loop variable " << s1 << " can not be indexed with " << s2 << ".");
            result = std::regex_replace(s1, std::regex((**it).name_), std::to_string((**it).i_));
            sgtLogDebug(LogLevel::VERBOSE) << "Expand loop: " << s1 << " -> " << result << std::endl;
        }
        else
        {
            // No matching loop, try parameters.
            YAML::Node nd;
            try
            {
                nd = parameters_.at(s1);
            }
            catch (std::out_of_range e)
            {
                sgtError("Parameter or loop " << s1 << " was not found.")
            }

            YAML::Node nd2;
            if (s2 != "")
            {
                if (nd.IsSequence())
                {
                    nd2 = nd[std::stoi(s2)];
                }
                else if (nd.IsMap())
                {
                    nd2 = nd[s2];
                }
                else
                {
                    sgtError("Can't index parameter " << s1 << ".");
                }

                sgtAssert(nd2, "For parameter " << s1 << ", index " << s2 << " doesn't exist.");
            }
            else
            {
                nd2 = nd;
            }
            result = nd2Str(nd2);
        }

        return result;
    }

    std::string ParserBase::nd2Str(const YAML::Node& nd) const
    {
        std::string result;
        if (!nd.IsScalar())
        {
            // Have to coerce List and Map nodes into the form of a string.
            std::ostringstream oss;
            oss << nd << std::endl;
            result = oss.str();
        }
        else
        {
            // For scalars, this way prevents extra newlines, etc. being inserted, which matters for strings.
            result = nd.as<std::string>();
        }
        return result;
    }
}
