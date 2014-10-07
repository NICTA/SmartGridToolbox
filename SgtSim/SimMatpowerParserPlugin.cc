#include "SimMatpowerParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/MatpowerParserPlugin.h>

namespace SmartGridToolbox
{
   void SimMatpowerParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
   {
      MatpowerParserPlugin mpParser;
      assertFieldPresent(nd, "network_id");
      string netwId = parser.expand<std::string>(nd["network_id"]);
      auto simNetw = sim.simComponent<SimNetwork>(netwId);

      Network tempNetw("temp", 100.0);
      mpParser.parse(nd, tempNetw, parser);

      for (auto& node : tempNetw.nodes())
      {
         const Bus& bus = dynamic_cast<Bus&>(*node->bus());
         auto simBus = sim.newSimComponent<SimBus>(bus);
         simNetw->addNode(simBus);
         for (auto& gen : node->gens())
         {
            const GenericGen& gGen = dynamic_cast<GenericGen&>(*gen);
            auto simGen = sim.newSimComponent<SimGenericGen>(gGen);
            simNetw->addGen(simGen, bus.id());
         }
         for (auto& zip : node->zips())
         {
            const GenericZip& gZip = dynamic_cast<GenericZip&>(*zip);
            auto simZip = sim.newSimComponent<SimGenericZip>(gZip);
            simNetw->addZip(simZip, bus.id());
         }
      }
      
      for (auto& arc : tempNetw.arcs())
      {
         const CommonBranch& cBranch = dynamic_cast<CommonBranch&>(*arc->branch());
         auto branch = sim.newSimComponent<SimCommonBranch>(cBranch);
         simNetw->addArc(branch, arc->node0()->bus()->id(), arc->node1()->bus()->id());
      }
   }
}
