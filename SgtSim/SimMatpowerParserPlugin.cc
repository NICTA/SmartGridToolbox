#include "SimMatpowerParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/MatpowerParserPlugin.h>

namespace Sgt
{
   void SimMatpowerParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
   {
      MatpowerParserPlugin mpParser;
      assertFieldPresent(nd, "network_id");
      string netwId = parser.expand<std::string>(nd["network_id"]);
      auto simNetw = sim.simComponent<SimNetwork>(netwId);

      Network tempNetw("temp", 100.0);
      mpParser.parse(nd, tempNetw, parser);

      // Now recreate the SimNetwork from the Network.
      for (auto& bus : tempNetw.busses())
      {
         auto simBus = sim.newSimComponent<SimBus>(*bus);
         simNetw->addBus(simBus);

         for (auto& gen : bus->gens())
         {
            auto gGen = dynamic_cast<const GenericGen&>(*gen);
            auto simGen = sim.newSimComponent<SimGenericGen>(gGen);
            simNetw->addGen(simGen, bus->id());
         }

         for (auto& zip : bus->zips())
         {
            auto gZip = dynamic_cast<const GenericZip&>(*zip);
            auto simZip = sim.newSimComponent<SimGenericZip>(gZip);
            simNetw->addZip(simZip, bus->id());
         }
      }

      for (auto& branch : tempNetw.branches())
      {
         auto cBranch = dynamic_cast<const CommonBranch&>(*branch);
         auto simCBranch = sim.newSimComponent<SimCommonBranch>(cBranch);
         simNetw->addBranch(simCBranch, branch->bus0()->id(), branch->bus1()->id());
      }
   }
}
