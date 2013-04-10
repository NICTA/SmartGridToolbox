#include "Parser.h"
#include "Output.h"
#include <string>

namespace SmartGridToolbox
{
   void Parser::Parse(const char * fname, Model & model)
   {
      std::cout << "Started parsing." << std::endl; 
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
      }
      else
      {
         Error("The configuration file must contain a \"global\" section.");
      }
      std::cout << "Finished parsing." << std::endl; 
      std::cout << "Name = " << model.GetName() << std::endl; 
   }
}
