#include "Common.h"
#include "Output.h"
#include "Parser.h"
#include "Simulation.h"
#include <string>

namespace SmartGridToolbox
{
   void Parser::Parse(const char * fname, Model & model, 
                      Simulation & simulation)
   {
      Message("Started parsing.");
      const YAML::Node & top = YAML::LoadFile(fname);

      ParseGlobal(top, model, simulation);
      ParseObjects(top, model, true);
      ParseObjects(top, model, false);

      Message("Finished parsing.");
      Message("Name = %s", model.Name().c_str());
      Message("Start time = %s", 
            to_simple_string(simulation.StartTime()).c_str());
      Message("End time = %s", 
            to_simple_string(simulation.EndTime()).c_str());
   }

   void Parser::ParseGlobal(const YAML::Node & top, Model & model,
                            Simulation & simulation)
   {
      if (const YAML::Node & nodeA = top["global"]) 
      {
         if (const YAML::Node & nodeB = nodeA["configuration_name"])
         {
            model.SetName(nodeB.as<std::string>());
         }
         else
         {
            model.SetName(std::string("null"));
         }

         if (const YAML::Node & nodeB = nodeA["start_time"])
         {
            try 
            {
               simulation.SetStartTime(
                     time_from_string(nodeB.as<std::string>()));
            }
            catch (...)
            {
               Error("Couldn't parse date.");
            }
         }
         else
         {
            Error("The configuration file must contain a start_time.");  
         }

         if (const YAML::Node & nodeB = nodeA["end_time"])
         {
            try 
            {
               simulation.SetEndTime(
                     time_from_string(nodeB.as<std::string>()));
            }
            catch (...)
            {
               Error("Couldn't parse date.");
            }
         }
         else
         {
            Error("The configuration file must contain an end_time.");  
         }
      }
      else
      {
         Error("The configuration file must contain a \"global\" section.");
      }
   }

   void Parser::ParseObjects(const YAML::Node & top, Model & model,
                             bool isPrototype)
   {
      if (const YAML::Node & nodeA = isPrototype ? top["prototypes"] 
                                                 : top["Objects"])
      {

      }
      
   }
}
