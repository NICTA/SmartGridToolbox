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
         auto bus = dynamic_cast<const Bus&>(*node->bus());
         auto simBus = sim.newSimComponent<SimBus>(bus);
         simNetw->addNode(simBus);
         for (auto& gen : node->gens())
         {
            auto gGen = dynamic_cast<const GenericGen&>(*gen);
            auto simGen = sim.newSimComponent<SimGenericGen>(gGen);
            simNetw->addGen(simGen, bus.id());
         }
         for (auto& zip : node->zips())
         {
            auto gZip = dynamic_cast<const GenericZip&>(*zip);
            auto simZip = sim.newSimComponent<SimGenericZip>(gZip);
            simNetw->addZip(simZip, bus.id());
         }
      }
      
      for (auto& arc : tempNetw.arcs())
      {
         auto cBranch = dynamic_cast<const CommonBranch&>(*arc->branch());
         auto branch = sim.newSimComponent<SimCommonBranch>(cBranch);
         simNetw->addArc(branch, arc->node0()->bus()->id(), arc->node1()->bus()->id());
      }
   }
}
