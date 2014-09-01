#include "GenericDcPowerSourceParser.h"

#include "GenericDcPowerSource.h"
#include "InverterBase.h"
#include "Simulation.h"

namespace SmartGridToolbox
{
   void GenericDcPowerSourceParser::parse(const YAML::Node& nd, Simulation& into) const
   {
      SGT_DEBUG(debug() << "GenericDcPowerSource : parse." << std::endl);

      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "inverter");
      assertFieldPresent(nd, "dc_power");

      string id = nd["id"].as<std::string>();
      auto& comp = into.newSimComponent<GenericDcPowerSource>(id);
      comp.setPDc(nd["dc_power"].as<double>());

      const std::string inverterStr = nd["inverter"].as<std::string>();
      auto inverterComp = into.simComponent<InverterBase>(inverterStr);
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
