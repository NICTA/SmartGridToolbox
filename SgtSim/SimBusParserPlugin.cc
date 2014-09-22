#include "SimBusParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/Bus.h>
#include <SgtCore/BusParserPlugin.h>

namespace SmartGridToolbox
{
   void SimBusParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const
   {
      BusParserPlugin busParser;
      auto bus = sim.newSimComponent<SimBus<Bus>>(busParser.parseBus(nd, state));

      assertFieldPresent(nd, "network_id");
      string netwId = state.expandName(nd["network_id"].as<std::string>());
      auto netw = sim.simComponent<SimNetwork>(netwId);
      netw->addNode(bus);
   }
}
