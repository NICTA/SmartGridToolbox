#include "SimpleDCPowerSource.h"

#include "Model.h"
#include "InverterBase.h"

namespace SmartGridToolbox
{
   void SimpleDCPowerSourceParser::parse(const YAML::Node & nd, Model & mod, const std::string & name,
                                         const ParserState & state) const
   {
      SGT_DEBUG(debug() << "SimpleDCPowerSource : parse." << std::endl);

      assertFieldPresent(nd, "inverter");
      assertFieldPresent(nd, "dc_power");
      auto & comp = mod.newComponent<SimpleDCPowerSource>(name);
      comp.setPDC(nd["dc_power"].as<double>());
   }

   void SimpleDCPowerSourceParser::postParse(const YAML::Node & nd, Model & mod, const std::string & name,
                                             const ParserState & state) const
   {
      SGT_DEBUG(debug() << "SimpleDCPowerSource : postParse." << std::endl);

      SimpleDCPowerSource & comp = *mod.componentNamed<SimpleDCPowerSource>(name);

      const std::string inverterStr = state.expandLoopRefs(nd["inverter"].as<std::string>());
      InverterBase * inverterComp = mod.componentNamed<InverterBase>(inverterStr);
      if (inverterComp != nullptr)
      {
         inverterComp->addDCPowerSource(comp);
      }
      else
      {
         error() << "For component " << name << ", inverter " << inverterStr << " was not found in the model." 
                 << std::endl;
         abort();
      }
   }
}
