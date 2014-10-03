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
                |   ('%' >> factor          [_val %= _1])
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

namespace YAML
{
   using namespace SmartGridToolbox;

   Node convert<Complex>::encode(const Complex& from)
   {
      Node nd;
      nd.push_back(complex2String(from));
      return nd;
   }

   bool convert<Complex>::decode(const Node& nd, Complex& to)
   {
      to = string2Complex(nd.as<std::string>());
      return true;
   }

   Node convert<Phase>::encode(const Phase& from)
   {
      Node nd;
      nd.push_back(phase2Str(from));
      return nd;
   }

   bool convert<Phase>::decode(const Node& nd, Phase& to)
   {
      to = str2Phase(nd.as<std::string>());
      return true;
   }

   Node convert<Phases>::encode(const Phases& from)
   {
      Node nd;
      for (const auto& phasePair : from)
      {
         nd.push_back(phase2Str(phasePair.first));
      }
      return nd;
   }
   bool convert<Phases>::decode(const Node& nd, Phases& to)
   {
      if(!nd.IsSequence())
      {
         return false;
      }
      else
      {
         int sz = nd.size();
         to = Phases();
         for (int i = 0; i < sz; ++i)
         {
            to |= nd[i].as<Phase>();
         }
      }
      return true;
   }

   Node convert<BusType>::encode(const BusType& from)
   {
      Node nd;
      nd.push_back(busType2Str(from));
      return nd;
   }

   bool convert<BusType>::decode(const Node& nd, BusType& to)
   {
      to = str2BusType(nd.as<std::string>());
      return true;
   }

   Node convert<Time>::encode(const Time& from)
   {
      Node nd;
      nd.push_back(posix_time::to_simple_string(from));
      return nd;
   }

   bool convert<Time>::decode(const Node& nd, Time& to)
   {
      to = posix_time::duration_from_string(nd.as<std::string>());
      return true;
   }

   Node convert<posix_time::ptime>::encode(const posix_time::ptime& from)
   {
      Node nd;
      nd.push_back(posix_time::to_simple_string(from));
      return nd;
   }

   bool convert<posix_time::ptime>::decode(const Node& nd, posix_time::ptime& to)
   {
      to = posix_time::time_from_string(nd.as<std::string>());
      return true;
   }

   template<typename T> Node convert<ublas::vector<T>>::encode(const ublas::vector<T>& from)
   {
      Node nd;
      for (const T& val : from) nd.push_back(val);
      return nd;
   }
   template Node convert<ublas::vector<double>>::encode(const ublas::vector<double>& from);
   template Node convert<ublas::vector<Complex>>::encode(const ublas::vector<Complex>& from);

   template<typename T> bool convert<ublas::vector<T>>::decode(const Node& nd, ublas::vector<T>& to)
   {
      if(!nd.IsSequence())
      {
         return false;
      }
      else
      {
         int sz = nd.size();
         to = ublas::vector<T>(sz);
         for (int i = 0; i < sz; ++i)
         {
            to(i) = nd[i].as<T>();
         }
      }
      return true;
   }
   template bool convert<ublas::vector<double>>::decode(const Node& nd, ublas::vector<double>& to);
   template bool convert<ublas::vector<Complex>>::decode(const Node& nd, ublas::vector<Complex>& to);

   template<typename T> Node convert<ublas::matrix<T>>::encode(const ublas::matrix<T>& from)
   {
      Node nd;
      for (int i = 0; i < from.size1(); ++i)
      {
         Node nd1;
         for (int k = 0; k < from.size2(); ++k)
         {
            nd1.push_back(from(i, k));
         }
         nd.push_back(nd1);
      }
      return nd;
   }
   template Node convert<ublas::matrix<double>>::encode(const ublas::matrix<double>& from);
   template Node convert<ublas::matrix<Complex>>::encode(const ublas::matrix<Complex>& from);

   template<typename T> bool convert<ublas::matrix<T>>::decode(const Node& nd, ublas::matrix<T>& to)
   {
      if(!nd.IsSequence())
      {
         return false;
      }
      else
      {
         int nrows = nd.size();
         if (nrows == 0)
         {
            std::cerr << "Matrix has zero rows in yaml." << std::endl;
            return false;
         }
         int ncols = nd[0].size();
         if (ncols == 0)
         {
            std::cerr << "Matrix has zero columns in yaml." << std::endl;
            return false;
         }
         for (int i = 1; i < nrows; ++i)
         {
            if (nd[i].size() != ncols)
            {
               std::cerr << "Ill-formed matrix in yaml." << std::endl;
               return false;
            }
         }
         to = ublas::matrix<T>(nrows, ncols);
         for (int i = 0; i < nrows; ++i)
         {
            for (int k = 0; k < nrows; ++k)
            {
               to(i, k) = nd[i][k].as<T>();
            }
         }
      }
      return true;
   }
   template bool convert<ublas::matrix<double>>::decode(const Node& nd, ublas::matrix<double>& to);
   template bool convert<ublas::matrix<Complex>>::decode(const Node& nd, ublas::matrix<Complex>& to);
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

      std::string name = ndLoopVar[0].as<std::string>();
      if (ndLoopVar.size() != 4)
      {
         Log().fatal() << "loop_variable expression is invalid. Format is [name, start, upper, stride]." << std::endl;
      }
      int start = expand<int>(ndLoopVar[1]);
      int upper = expand<int>(ndLoopVar[2]);
      int stride = expand<int>(ndLoopVar[3]);
      loops_.push_back({name, start, upper, stride});

      return loops_.back();
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

      auto it = std::find_if(loops_.begin(), loops_.end(), [&s1](const ParserLoop& l){return l.name_ == s1;});
      if (it != loops_.end())
      {
         // We matched a loop variable. Make sure there is no index!
         if (s2 != "")
         {
            Log().fatal() << "Loop variable " << s1 << " can not be indexed with " << s2 << "." << std::endl;
         }
         result = std::regex_replace(s1, std::regex(it->name_), std::to_string(it->i_));
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
