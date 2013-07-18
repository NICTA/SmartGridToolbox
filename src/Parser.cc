#include "Common.h"
#include "Model.h"
#include "Output.h"
#include "Parser.h"
#include "Simulation.h"
#include <string>

namespace YAML
{
   Node convert<UblasVector<double>>::encode(const UblasVector<double> & from) 
   {
      Node nd;
      for (double val : from) nd.push_back(val);
      return nd;
   }

   bool convert<UblasVector<double>>::decode(const Node& nd, UblasVector<double> & to)
   {
      if(!nd.IsSequence())
      {
         return false;
      }
      else
      {
         int sz = nd.size();
         to = UblasVector<double>(sz);
         for (int i = 0; i < sz; ++i)
         {
            to(i) = nd[i].as<double>();
         }
      }
      return true;
   }

   Node convert<UblasVector<Complex>>::encode(const UblasVector<Complex> & from)
   {
      Node nd;
      for (Complex val : from)
      {
         Node nd1;
         nd1.push_back(val.real());
         nd1.push_back(val.imag());
         nd.push_back(nd1);
      }
      return nd;
   }

   bool convert<UblasVector<Complex>>::decode(const Node& nd, UblasVector<Complex> & to)
   {
      if(!nd.IsSequence())
      {
         return false;
      }
      else
      {
         int sz = nd.size();
         to = UblasVector<Complex>(sz);
         for (int i = 0; i < sz; ++i)
         {
            double re = nd[0].as<double>();
            double im = nd[1].as<double>();
            to(i) = Complex(re, im);
         }
      }
      return true;
   }

   Node convert<UblasMatrix<double>>::encode(const UblasMatrix<double> & from)
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

   bool convert<UblasMatrix<double>>::decode(const Node& nd, UblasMatrix<double> & to)
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
         to = UblasMatrix<double>(nrows, ncols);
         for (int i = 0; i < nrows; ++i)
         {
            for (int k = 0; k < nrows; ++k)
            {
               to(i, k) = nd[i][k].as<double>();
            }
         }
      }
      return true;
   }

   Node convert<UblasMatrix<Complex>>::encode(const UblasMatrix<Complex> & from)
   {
      Node nd;
      for (int i = 0; i < from.size1(); ++i)
      {
         Node nd1;
         for (int k = 0; k < from.size2(); ++k)
         {
            Node nd2;
            nd2.push_back(from(i, k).real());
            nd2.push_back(from(i, k).imag());
            nd1.push_back(nd2);
         }
         nd.push_back(nd1);
      }
      return nd;
   }

   bool convert<UblasMatrix<Complex>>::decode(const Node& nd, UblasMatrix<Complex> & to)
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
         to = UblasMatrix<Complex>(nrows, ncols);
         for (int i = 0; i < nrows; ++i)
         {
            for (int k = 0; k < nrows; ++k)
            {
               to(i, k) = {nd[i][k][0].as<double>(), nd[i][k][1].as<double>()};
            }
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
