#include "GenericDcPowerSourceParserPlugin.h"

#include "DcPowerSource.h"
#include "Inverter.h"
#include "Simulation.h"

namespace Sgt
{
   void GenericDcPowerSourceParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "inverter");
      assertFieldPresent(nd, "dc_power");

      string id = parser.expand<std::string>(nd["id"]);
      auto comp = sim.newSimComponent<GenericDcPowerSource>(id);
      comp->setPDc(parser.expand<double>(nd["dc_power"]));

      const std::string inverterStr = parser.expand<std::string>(nd["inverter"]);
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
