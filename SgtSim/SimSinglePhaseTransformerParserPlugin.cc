#include "SimSinglePhaseTransformerParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/SinglePhaseTransformer.h>
#include <SgtCore/SinglePhaseTransformerParserPlugin.h>

#include <memory>

namespace SmartGridToolbox
{
   void SimSinglePhaseTransformerParserPlugin::parse(const YAML::Node& nd, Simulation& sim,
         const ParserBase& parser) const
   {
      SinglePhaseTransformerParserPlugin transParser;
      auto trans = sim.newSimComponent<SimSinglePhaseTransformer>(*transParser.parseSinglePhaseTransformer(nd, parser));
      
      assertFieldPresent(nd, "network_id");
      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");

      string netwId = parser.expand<std::string>(nd["network_id"]);
      std::string bus0Id = parser.expand<std::string>(nd["bus_0_id"]);
      std::string bus1Id = parser.expand<std::string>(nd["bus_1_id"]);

      auto netw = sim.simComponent<SimNetwork>(netwId);

      netw->addArc(trans, bus0Id, bus1Id);
   }
}
