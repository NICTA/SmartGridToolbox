#include "Common.h"
#include "Model.h"
#include "Output.h"
#include "Parser.h"
#include "Simulation.h"
#include <string>

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
      }
      message("Parsing components. Completed.");
   }
}
