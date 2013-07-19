#include "Common.h"
#include "Model.h"
#include "Output.h"
#include "Parser.h"
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
   void Parser::parse(const char * fname, Model & model, 
                      Simulation & simulation)
   {
      message("Started parsing.");
      const YAML::Node & top = YAML::LoadFile(fname);

      message("Parsing global.");
      parseGlobal(top, model, simulation);
      message("Parsed global.");
      message("Parsing prototypes.");
      parseComponents(top, model, true);
      message("Parsed prototypes.");
      message("Parsing objects.");
      parseComponents(top, model, false);
      message("Parsed objects.");

      message("Finished parsing.");
      message("Name = %s", model.getName().c_str());
      message("Start time = %s", 
            to_simple_string(simulation.getStartTime()).c_str());
      message("End time = %s", 
            to_simple_string(simulation.getEndTime()).c_str());
   }

   void Parser::parseGlobal(const YAML::Node & top, Model & model,
                            Simulation & simulation)
   {
      if (const YAML::Node & nodeA = top["global"]) 
      {
         if (const YAML::Node & nodeB = nodeA["configuration_name"])
         {
            model.setName(nodeB.as<std::string>());
         }
         else
         {
            model.setName(std::string("null"));
         }

         if (const YAML::Node & nodeB = nodeA["start_time"])
         {
            try 
            {
               simulation.setStartTime(
                     time_from_string(nodeB.as<std::string>()));
            }
            catch (...)
            {
               error("Couldn't parse date.");
            }
         }
         else
         {
            error("The configuration file must contain a start_time.");  
         }

         if (const YAML::Node & nodeB = nodeA["end_time"])
         {
            try 
            {
               simulation.setEndTime(
                     time_from_string(nodeB.as<std::string>()));
            }
            catch (...)
            {
               error("Couldn't parse date.");
            }
         }
         else
         {
            error("The configuration file must contain an end_time.");  
         }
      }
      else
      {
         error("The configuration file must contain a \"global\" section.");
      }
   }

   void Parser::parseComponents(const YAML::Node & top, Model & model,
                                bool isPrototype)
   {
      message("Parsing components. Starting.");
      if (const YAML::Node & compsNode = isPrototype ? top["prototypes"] 
                                                     : top["objects"])
      {
         for (auto it = compsNode.begin(); it != compsNode.end(); ++it)
         {
            std::string name = it->first.as<std::string>();
            message("Parsing component %s.", name.c_str());
            const ComponentParser * compParser = getComponentParser(name);
            if (compParser == nullptr)
            {
               warning("I don't know how to parse component %s.", name.c_str());
            }
            else
            {
               compParser->parse(it->second, model);
            }
         }
         for (auto it = compsNode.begin(); it != compsNode.end(); ++it)
         {
            std::string name = it->first.as<std::string>();
            message("Post parsing component %s.", name.c_str());
            const ComponentParser * compParser = getComponentParser(name);
            if (compParser == nullptr)
            {
               warning("I don't know how to parse component %s.", name.c_str());
            }
            else
            {
               compParser->postParse(it->second, model);
            }
         }
         message("Parsing components. Completed.");
      }
   }
}
