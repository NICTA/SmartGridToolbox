#include "SimpleDCPowerSource.h"

#include "Model.h"
#include "InverterBase.h"

namespace SmartGridToolbox
{
   void SimpleDCPowerSourceParser::parse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "SimpleDCPowerSource : parse." << std::endl);

      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "inverter");
      const std::string nameStr = nd["name"].as<std::string>();
      mod.newComponent<SimpleDCPowerSource>(nameStr);
   }

   void SimpleDCPowerSourceParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "SimpleDCPowerSource : postParse." << std::endl);

      const std::string nameStr = nd["name"].as<std::string>();
      SimpleDCPowerSource & comp = *mod.getComponentNamed<SimpleDCPowerSource>(nameStr);

      const std::string inverterStr = nd["inverter"].as<std::string>();
      InverterBase * inverterComp = mod.getComponentNamed<InverterBase>(inverterStr);
      if (inverterComp != nullptr)
      {
         inverterComp->addDCPowerSource(comp);
      }
      else
      {
         error() << "For component " << nameStr << ", inverter " << inverterStr << " was not found in the model." 
                 << std::endl;
         abort();
      }
   }
}
