#include "Common.h"
#include "Model.h"
#include "Parser.h"
#include "RegisterComponentParsers.h"
#include "Simulation.h"
#include <string>
#include <boost/algorithm/string.hpp>

namespace YAML
{
   using namespace SmartGridToolbox;

   Node convert<Complex>::encode(const Complex & from)
   {
      Node nd;
      nd.push_back(complex2String(from));
      return nd;
   }

   bool convert<Complex>::decode(const Node & nd, Complex & to)
   {
      to = string2Complex(nd.as<std::string>());
      return true;
   }

   Node convert<Phase>::encode(const Phase & from)
   {
      Node nd;
      nd.push_back(phase2Str(from));
      return nd;
   }

   bool convert<Phase>::decode(const Node & nd, Phase & to)
   {
      to = str2Phase(nd.as<std::string>());
      return true;
   }

   Node convert<Phases>::encode(const Phases & from)
   {
      Node nd;
      for (const auto & phasePair : from)
      {
         nd.push_back(phase2Str(phasePair.first)); 
      }
      return nd;
   }
   bool convert<Phases>::decode(const Node & nd, Phases & to)
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

   Node convert<BusType>::encode(const BusType & from)
   {
      Node nd;
      nd.push_back(busType2Str(from));
      return nd;
   }

   bool convert<BusType>::decode(const Node & nd, BusType & to)
   {
      to = str2BusType(nd.as<std::string>());
      return true;
   }

   Node convert<Time>::encode(const Time & from)
   {
      Node nd;
      nd.push_back(to_simple_string(from));
      return nd;
   }

   bool convert<Time>::decode(const Node & nd, Time & to)
   {
      to = duration_from_string(nd.as<std::string>()); 
      return true;
   }

   Node convert<ptime>::encode(const ptime & from)
   {
      Node nd;
      nd.push_back(to_simple_string(from));
      return nd;
   }

   bool convert<ptime>::decode(const Node & nd, ptime & to)
   {
      to = time_from_string(nd.as<std::string>()); 
      return true;
   }

   template<typename T> Node convert<UblasVector<T>>::encode(const UblasVector<T> & from)
   {
      Node nd;
      for (const T & val : from) nd.push_back(val);
      return nd;
   }
   template Node convert<UblasVector<double>>::encode(const UblasVector<double> & from);
   template Node convert<UblasVector<Complex>>::encode(const UblasVector<Complex> & from);

   template<typename T> bool convert<UblasVector<T>>::decode(const Node & nd, UblasVector<T> & to)
   {
      if(!nd.IsSequence())
      {
         return false;
      }
      else
      {
         int sz = nd.size();
         to = UblasVector<T>(sz);
         for (int i = 0; i < sz; ++i)
         {
            to(i) = nd[i].as<T>();
         }
      }
      return true;
   }
   template bool convert<UblasVector<double>>::decode(const Node & nd, UblasVector<double> & to);
   template bool convert<UblasVector<Complex>>::decode(const Node & nd, UblasVector<Complex> & to);

   template<typename T> Node convert<UblasMatrix<T>>::encode(const UblasMatrix<T> & from)
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
   template Node convert<UblasMatrix<double>>::encode(const UblasMatrix<double> & from);
   template Node convert<UblasMatrix<Complex>>::encode(const UblasMatrix<Complex> & from);

   template<typename T> bool convert<UblasMatrix<T>>::decode(const Node & nd, UblasMatrix<T> & to)
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
         to = UblasMatrix<T>(nrows, ncols);
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
   template bool convert<UblasMatrix<double>>::decode(const Node & nd, UblasMatrix<double> & to);
   template bool convert<UblasMatrix<Complex>>::decode(const Node & nd, UblasMatrix<Complex> & to);
}

namespace SmartGridToolbox
{
   std::string ParserState::expandName(const std::string & target) const
   {
      std::string result(target);
      for (const ParserLoop & loop : loops_)
      {
         std::string search = "${" + loop.name_ + "}";
         std::string replace = std::to_string(loop.i_);
         boost::replace_all(result, search, replace);
      }
      return result;
   }

   void assertFieldPresent(const YAML::Node & nd, const std::string & field)
   {
      if (!(nd[field]))
      {
         error() << "Parsing: " << field << " field not present." << std::endl;
         abort();
      }
   };

   void Parser::parse(const char * fname, Model & model, Simulation & simulation)
   {
      message() << "Started parsing." << std::endl;
      const YAML::Node & top = YAML::LoadFile(fname);

      SGT_DEBUG(debug() << "Parsing global." << std::endl);
      parseGlobal(top, model, simulation);
      SGT_DEBUG(debug() << "Parsed global." << std::endl);

      const YAML::Node & objsNode = top["objects"];
      if (objsNode)
      {
         SGT_DEBUG(debug() << "Parsing objects." << std::endl);
         ParserState s;
         parseComponents(objsNode, s, model, false);
         parseComponents(objsNode, s, model, true);
         SGT_DEBUG(debug() << "Parsed objects." << std::endl);
      }

      message() << "Finished parsing " << model.name() << "." << std::endl;
      message() << "Start time (local) = " << localTime(simulation.startTime(), model.timezone()) << std::endl;
      message() << "Start time (UTC)   = " << utcTime(simulation.startTime()) << std::endl;
      message() << "End time (local)   = " << localTime(simulation.endTime(), model.timezone()) << std::endl;
      message() << "End time (UTC)     = " << utcTime(simulation.endTime()) << std::endl;
      message() << "timezone           = " << model.timezone()->to_posix_string() << std::endl;
      message() << "lat_long           = " << model.latLong().lat_ << ", " << model.latLong().long_ << "." << std::endl;
   }
   
   Parser::Parser()
   {
      registerComponentParsers(*this);
   }

   void Parser::parseGlobal(const YAML::Node & top, Model & model,
                            Simulation & simulation)
   {
      assertFieldPresent(top, "global");
      const YAML::Node & nodeGlobal = top["global"];

      assertFieldPresent(nodeGlobal, "start_time");
      assertFieldPresent(nodeGlobal, "end_time");

      if (const YAML::Node & nodeConfig = nodeGlobal["configuration_name"])
      {
         model.setName(nodeConfig.as<std::string>());
      }
      else
      {
         model.setName(std::string("null"));
      }

      if (const YAML::Node & nodeTz = nodeGlobal["timezone"])
      {
         try 
         {
            model.setTimezone(time_zone_ptr(new posix_time_zone(nodeTz.as<std::string>())));
         }
         catch (...)
         {
            error() << "Couldn't parse timezone " << nodeTz.as<std::string>() << "." << std::endl;
            abort();
         }
      }

      const YAML::Node & nodeStart = nodeGlobal["start_time"];
      try 
      {
         simulation.setStartTime(parseTime(nodeStart, model));
      }
      catch (...)
      {
         error() << "Couldn't parse start date " << nodeStart.as<std::string>() << "." << std::endl;
         abort();
      }

      const YAML::Node & nodeEnd = nodeGlobal["end_time"];
      try 
      {
         simulation.setEndTime(parseTime(nodeEnd, model));
      }
      catch (...)
      {
         error() << "Couldn't parse end date " << nodeEnd.as<std::string>() << "." << std::endl;
         abort();
      }

      if (const YAML::Node & nodeLatLong = nodeGlobal["lat_long"])
      {
         try 
         {
            std::vector<double> llvec = nodeLatLong.as<std::vector<double>>();
            if (llvec.size() != 2)
            {
               throw;
            };
            model.setLatLong({llvec[0], llvec[1]});
         }
         catch (...)
         {
            error() << "Couldn't parse lat_long " << nodeLatLong.as<std::string>() << "." << std::endl;
            abort();
         }
      }

   }

   void Parser::parseComponents(const YAML::Node & node, ParserState & state, Model & model, bool isPostParse)
   {
      message() << "Parsing components. Starting." << std::endl;
      for (const auto & compPair : node)
      {
         std::string nodeType = compPair.first.as<std::string>();
         const YAML::Node & nodeVal = compPair.second;;
         if (nodeType == "loop")
         {
            std::string loopName = nodeVal["name"].as<std::string>();
            int loopCount = nodeVal["count"].as<int>();
            const YAML::Node & loopBody = nodeVal["body"];
            for (state.pushLoop(loopName); state.topLoopVal() < loopCount; state.incrTopLoop())
            {
               parseComponents(loopBody, state, model, isPostParse);
            }
            state.popLoop();
         }
         else
         {
            message() << "Parsing plugin " <<  nodeType << "." << std::endl;
            const ComponentParser * compParser = componentParser(nodeType);
            if (compParser == nullptr)
            {
               warning() << "I don't know how to parse component " << nodeType << std::endl;
            }
            else if (isPostParse)
            {
               compParser->postParse(nodeVal, model, state);
            }
            else
            {
               compParser->parse(nodeVal, model, state);
            }
         }
      }
      message() << "Parsing components. Completed." << std::endl;
   }
}
