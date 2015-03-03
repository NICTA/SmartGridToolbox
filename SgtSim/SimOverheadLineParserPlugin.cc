#include "SimOverheadLineParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/OverheadLine.h>
#include <SgtCore/OverheadLineParserPlugin.h>

#include <memory>

namespace Sgt
{
   void SimOverheadLineParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
   {
      OverheadLineParserPlugin ohlParser;
      auto simOhl = sim.newSimComponent<SimOverheadLine>(*ohlParser.parseOverheadLine(nd, parser));

      assertFieldPresent(nd, "network_id");
      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");

      string netwId = parser.expand<std::string>(nd["network_id"]);
      std::string bus0Id = parser.expand<std::string>(nd["bus_0_id"]);
      std::string bus1Id = parser.expand<std::string>(nd["bus_1_id"]);

      auto netw = sim.simComponent<SimNetwork>(netwId);

      netw->addBranch(std::move(simOhl), bus0Id, bus1Id);
   }
}
