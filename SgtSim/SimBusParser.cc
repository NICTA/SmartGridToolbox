#include "SimBusParser.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/Bus.h>
#include <SgtCore/BusParser.h>

namespace SmartGridToolbox
{
   void SimBusParser::parse(const YAML::Node& nd, Simulation& into) const
   {
      SGT_DEBUG(debug() << "SimBus : parse." << std::endl);
      
      BusParser busParser;
      auto bus = std::make_shared<SimBus<Bus>>(busParser.parseBus(nd));

      assertFieldPresent(nd, "network_id");
      string netwId = nd["network_id"].as<std::string>();
      auto netw = into.simComponent<SimNetwork>(netwId);
      netw->addNode(bus);
   }
}
