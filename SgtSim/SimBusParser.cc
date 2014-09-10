#include "SimBusParser.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/Bus.h>
#include <SgtCore/BusParser.h>

namespace SmartGridToolbox
{
   void SimBusParser::parse(const YAML::Node& nd, Simulation& sim) const
   {
      BusParser busParser;
      auto bus = sim.newSimComponent<SimBus<Bus>>(busParser.parseBus(nd));

      assertFieldPresent(nd, "network_id");
      string netwId = nd["network_id"].as<std::string>();
      auto netw = sim.simComponent<SimNetwork>(netwId);
      netw->addNode(bus);
   }
}
