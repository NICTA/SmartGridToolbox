#include "Common.h"
#include "Model.h"
#include "Parser.h"
#include "RegisterComponentParsers.h"
#include "Simulation.h"
#include <string>

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
}

namespace SmartGridToolbox
{
   void Parser::parse(const char * fname, Model & model, 
                      Simulation & simulation)
   {
      message() << "Started parsing." << std::endl;
      const YAML::Node & top = YAML::LoadFile(fname);

      SGT_DEBUG(debug() << "Parsing global." << std::endl);
      parseGlobal(top, model, simulation);
      SGT_DEBUG(debug() << "Parsed global." << std::endl);
      SGT_DEBUG(debug() << "Parsing objects." << std::endl);
      parseComponents(top, model);
      SGT_DEBUG(debug() << "Parsed objects." << std::endl);

      message() << "Finished parsing." << std::endl;
      message() << "Name = " << model.name() << std::endl;
      message() << "UTC start time = " << simulation.startTime() << std::endl;
      message() << "UTC end time = " << simulation.endTime() << std::endl;
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

      const YAML::Node & nodeStart = nodeGlobal["start_time"];
      try 
      {
         simulation.setStartTime(model.utcTime(time_from_string(nodeStart.as<std::string>())));
      }
      catch (...)
      {
         error() << "Couldn't parse start date " << nodeStart.as<std::string>() << "." << std::endl;
         abort();
      }

      const YAML::Node & nodeEnd = nodeGlobal["end_time"];
      try 
      {
         simulation.setEndTime(model.utcTime(time_from_string(nodeEnd.as<std::string>())));
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
   }

   void Parser::parseComponents(const YAML::Node & top, Model & model)
   {
      message() << "Parsing components. Starting." << std::endl;
      if (const YAML::Node & compsNode = top["objects"])
      {
         for (const auto & compPair : compsNode)
         {
            std::string name = compPair.first.as<std::string>();
            message() << "Parsing component " <<  name << std::endl;
            const ComponentParser * compParser = componentParser(name);
            if (compParser == nullptr)
            {
               warning() << "I don't know how to parse component " << name << std::endl;
            }
            else
            {
               compParser->parse(compPair.second, model);
            }
         }
         for (const auto & compPair : compsNode)
         {
            std::string name = compPair.first.as<std::string>();
            message() << "Post parsing component " << name << std::endl;
            const ComponentParser * compParser = componentParser(name);
            if (compParser == nullptr)
            {
               warning() << "I don't know how to parse component " << name << std::endl;
            }
            else
            {
               compParser->postParse(compPair.second, model);
            }
         }
         message() << "Parsing components. Completed." << std::endl;
      }
   }
}
