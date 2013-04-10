#include "Parser.h"
#include "Common.h"
#include "Output.h"
#include <string>

namespace SmartGridToolbox
{
   void Parser::Parse(const char * fname, Model & model)
   {
      Message("Started parsing.");
      const YAML::Node & config = YAML::LoadFile(fname);

      if (const YAML::Node & nodeA = config["global"]) 
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
               model.SetStartTime(time_from_string(nodeB.as<std::string>()));
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
               model.SetEndTime(time_from_string(nodeB.as<std::string>()));
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
      Message("Finished parsing.");
      Message("Name = %s", model.GetName().c_str());
      Message("Start time = %s", 
            to_simple_string(model.GetStartTime()).c_str());
      Message("End time = %s", 
            to_simple_string(model.GetEndTime()).c_str());
   }
}
