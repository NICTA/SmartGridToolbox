#include "SimOverheadLineParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/OverheadLine.h>
#include <SgtCore/OverheadLineParserPlugin.h>

#include <memory>

namespace SmartGridToolbox
{
   void SimOverheadLineParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const
   {
      OverheadLineParserPlugin ohlParser;
      auto ohl = sim.newSimComponent<SimBranch<OverheadLine>>(ohlParser.parseOverheadLine(nd, state));

      assertFieldPresent(nd, "network_id");
      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");

      string netwId = state.expandName(nd["network_id"].as<std::string>());
      std::string bus0Id = state.expandName(nd["bus_0_id"].as<std::string>());
      std::string bus1Id = state.expandName(nd["bus_1_id"].as<std::string>());

      auto netw = sim.simComponent<SimNetwork>(netwId);

      netw->addArc(ohl, bus0Id, bus1Id);
   }
}
