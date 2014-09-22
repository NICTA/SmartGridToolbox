#include "GenericDcPowerSourceParserPlugin.h"

#include "DcPowerSource.h"
#include "Inverter.h"
#include "Simulation.h"

namespace SmartGridToolbox
{
   void GenericDcPowerSourceParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "inverter");
      assertFieldPresent(nd, "dc_power");

      string id = state.expandName(nd["id"].as<std::string>());
      auto comp = sim.newSimComponent<GenericDcPowerSource>(id);
      comp->setPDc(nd["dc_power"].as<double>());

      const std::string inverterStr = nd["inverter"].as<std::string>();
      auto inverterComp = sim.simComponent<InverterAbc>(inverterStr);
      if (inverterComp != nullptr)
      {
         inverterComp->addDcPowerSource(comp);
      }
      else
      {
         Log().fatal() << "For component " << id << ", inverter "
                 << inverterStr << " was not found in the model." << std::endl;
      }
   }
}
