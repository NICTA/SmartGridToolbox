#include "Parser.h"

#include "Common.h"
#include "PowerFlow.h"
#include "RegisterParserPlugins.h"

#include <string>
#include <boost/algorithm/string.hpp>

namespace YAML
{
   using namespace LibPowerFlow;

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

namespace LibPowerFlow
{
   void assertFieldPresent(const YAML::Node& nd, const std::string& field)
   {
      if (!(nd[field]))
      {
         error() << "Parsing: " << field << " field not present." << std::endl;
         abort();
      }
   };

   void Parser::parse(const std::string& fname, Network& netw)
   {
      message() << "Started parsing file " << fname << "." << std::endl;

      auto top = YAML::LoadFile(fname);
      if (top.size() == 0)
      {
         error() << "Parsing file " << fname << ". File is empty or doesn't exist." << std::endl;
         abort();
      }
      
      message() << "Parsing plugins. Starting." << std::endl;
      for (const auto& subnode : top)
      {
         std::string nodeType = subnode.first.as<std::string>();
         const YAML::Node& nodeVal = subnode.second;;
         message() << "Parsing plugin " <<  nodeType << "." << std::endl;
         const ParserPlugin* plugin = plugins_[nodeType];
         if (plugin == nullptr)
         {
            warning() << "I don't know how to parse plugin " << nodeType << std::endl;
         }
         else
         {
            plugin->parse(nodeVal, netw);
         }
      }
      message() << "Parsing plugins. Completed." << std::endl;

      message() << "Finished parsing file " << fname << "." << std::endl;
   }

   Parser::Parser()
   {
      registerParserPlugins(*this);
   }
}
