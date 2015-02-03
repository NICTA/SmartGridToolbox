#include "SimYyTransformerParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/YyTransformer.h>
#include <SgtCore/YyTransformerParserPlugin.h>

#include <memory>

namespace SmartGridToolbox
{
   void SimYyTransformerParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
   {
      YyTransformerParserPlugin transParser;
      auto trans = sim.newSimComponent<SimYyTransformer>(*transParser.parseYyTransformer(nd, parser));
      
      assertFieldPresent(nd, "network_id");
      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");

      string netwId = parser.expand<std::string>(nd["network_id"]);
      std::string bus0Id = parser.expand<std::string>(nd["bus_0_id"]);
      std::string bus1Id = parser.expand<std::string>(nd["bus_1_id"]);

      auto netw = sim.simComponent<SimNetwork>(netwId);

      netw->addBranch(trans, bus0Id, bus1Id);
   }
}
