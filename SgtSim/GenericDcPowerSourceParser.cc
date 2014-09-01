#include "GenericDcPowerSourceParser.h"

#include "GenericDcPowerSource.h"
#include "InverterBase.h"

namespace SmartGridToolbox
{
   void GenericDcPowerSourceParser::parse(const YAML::Node& nd, Simulation& data) const
   {
      SGT_DEBUG(debug() << "GenericDcPowerSource : parse." << std::endl);

      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "inverter");
      assertFieldPresent(nd, "dc_power");

      string id = nd["id"].as<std::string>();
      auto& comp = data.newSimComponent<GenericDcPowerSource>(id);
      comp.setPDc(nd["dc_power"].as<double>());
   }

   void GenericDcPowerSourceParser::postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "GenericDcPowerSource : postParse." << std::endl);

      string id = state.expandName(nd["id"].as<std::string>());
      GenericDcPowerSource& comp = *mod.component<GenericDcPowerSource>(id);

      const std::string inverterStr = state.expandName(nd["inverter"].as<std::string>());
      InverterBase* inverterComp = mod.component<InverterBase>(inverterStr);
      if (inverterComp != nullptr)
      {
         inverterComp->addDcPowerSource(comp);
      }
      else
      {
         error() << "For component " << id << ", inverter "
                 << inverterStr << " was not found in the model." << std::endl;
         abort();
      }
   }
}
