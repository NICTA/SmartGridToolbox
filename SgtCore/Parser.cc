#include "NetworkParser.h"

#include "Common.h"
#include "PowerFlow.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include <regex>
#include <string>

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
   std::string ParserState::expandName(const std::string & target) const
   {
      std::string result;

      std::regex r("\\$\\{.*?\\}");

      std::sregex_iterator begin(target.begin(), target.end(), r);
      std::sregex_iterator end;

      if (begin == end)
      {
         result = target;
      }
      else
      {
         for (auto it = begin; it != end; ++it)
         {
            result += it->prefix();
            std::string substr = it->str().substr(2, it->str().size()-3);
            for (const ParserLoop & loop : loops_)
            {
               substr = std::regex_replace(substr, std::regex(loop.name_), std::to_string(loop.i_));
               int calcResult;
               std::string::const_iterator i1 = substr.begin();
               std::string::const_iterator i2 = substr.end();
               bool ok = phrase_parse(i1, i2, calc, space, calcResult);
               if (!ok)
               {
                  Log().fatal() << "Ill-formed expression " << target << " for variables" << std::endl;
               }
               result += std::to_string(calcResult); 
            }
         }
      }
      return result;
   }

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
}
