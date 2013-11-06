#include <SmartGridToolbox/SimpleDcPowerSource.h>

#include <SmartGridToolbox/Model.h>
#include <SmartGridToolbox/InverterBase.h>

namespace SmartGridToolbox
{
   void SimpleDcPowerSourceParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "SimpleDcPowerSource : parse." << std::endl);

      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "inverter");
      assertFieldPresent(nd, "dc_power");

      string name = state.expandName(nd["name"].as<std::string>());
      auto & comp = mod.newComponent<SimpleDcPowerSource>(name);

      comp.setPDc(nd["dc_power"].as<double>());
   }

   void SimpleDcPowerSourceParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "SimpleDcPowerSource : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      SimpleDcPowerSource & comp = *mod.componentNamed<SimpleDcPowerSource>(name);

      const std::string inverterStr = state.expandName(nd["inverter"].as<std::string>());
      InverterBase * inverterComp = mod.componentNamed<InverterBase>(inverterStr);
      if (inverterComp != nullptr)
      {
         inverterComp->addDcPowerSource(comp);
      }
      else
      {
         error() << "For component " << name << ", inverter " 
                 << inverterStr << " was not found in the model." << std::endl;
         abort();
      }
   }
}
