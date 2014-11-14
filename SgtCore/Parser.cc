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

namespace SmartGridToolbox
{

   void assertFieldPresent(const YAML::Node& nd, const std::string& field)
   {
      if (!(nd[field]))
      {
         Log().fatal() << "Parsing: " << field << " field not present." << std::endl;
      }
   };
   
   YAML::Node getTopNode(const std::string& fname)
   {
      Log().message() << "Opening file " << fname << " for parsing." << std::endl;
      auto top = YAML::LoadFile(fname);
      if (top.size() == 0)
      {
         Log().fatal() << "File " << fname << " is empty or doesn't exist." << std::endl;
      }
      return top;
   }

   ParserBase::ParserLoop& ParserBase::parseLoop(const YAML::Node& nd)
   {
      assertFieldPresent(nd, "loop_variable");
      assertFieldPresent(nd, "loop_body");

      const YAML::Node& ndLoopVar = nd["loop_variable"];
      const YAML::Node& ndBody = nd["loop_body"];

      std::string name = ndLoopVar[0].as<std::string>();
      if (ndLoopVar.size() != 4)
      {
         Log().fatal() << "loop_variable expression is invalid. Format is [name, start, upper, stride]." << std::endl;
      }
      int start = expand<int>(ndLoopVar[1]);
      int upper = expand<int>(ndLoopVar[2]);
      int stride = expand<int>(ndLoopVar[3]);
      loops_.push_back(std::unique_ptr<ParserLoop>(new ParserLoop{name, start, upper, stride, ndBody}));

      return *loops_.back();
   }
         

   namespace 
   {
      const std::regex expr(                 // Submatch 0: whole expr, with <>.
               "<"
               "("                           // Submatch 1: whole expr body, without <>.
                  "("                        // Submatch 2: whole math expr body.
                     "[0-9+\\-*/%() \t]+"
                  ")|"
                  "("                        // Submatch 3: whole var/loop expr body.
                     "("                     // Submatch 4: non-index part of var/loop expr body.
                        "[a-zA-Z_][\\w]*"
                     ")+"
                     "("                     // Submatch 5: index part of var/loop expr body, with ().
                        "\\("
                        "("                  // Submatch 6: index part of var/loop expr, without ().
                           "[a-zA-Z0-9_]*"
                        ")"
                        "\\)"
                     ")?"
                  ")"
               ")"
               ">"
            );
   }

   std::string ParserBase::expandString(const std::string& str) const
   {
      std::string result;

      // Iterator over expansion expressions in the target.
      const std::sregex_iterator begin(str.begin(), str.end(), expr);
      const std::sregex_iterator end;

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
      if (!ok)
      {
         Log().fatal() << "Ill-formed expression " << str << std::endl;
      }

      std::string result = std::to_string(calcResult); 

      return result;
   }

   std::string ParserBase::expandLoopOrParameterExpressionBody(const std::string& s1, const std::string& s2) const
   {
      std::string result;

      auto it = std::find_if(loops_.begin(), loops_.end(),
            [&s1](const std::unique_ptr<ParserLoop>& l){return l->name_ == s1;});
      if (it != loops_.end())
      {
         // We matched a loop variable. Make sure there is no index!
         if (s2 != "")
         {
            Log().fatal() << "Loop variable " << s1 << " can not be indexed with " << s2 << "." << std::endl;
         }
         result = std::regex_replace(s1, std::regex((**it).name_), std::to_string((**it).i_));
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
            Log().fatal() << "Parameter or loop " << s1 << " was not found." << std::endl;
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
               Log().fatal() << "Can't index parameter " << s1 << "." << std::endl;
            }

            if (!nd2)
            {
               Log().fatal() << "For parameter " << s1 << ", index " << s2 << " doesn't exist." << std::endl;
            }
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
