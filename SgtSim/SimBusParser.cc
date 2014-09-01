#include "SimBusParser.h"

#include "SimNetwork.h"

#include <SgtCore/Bus.h>
#include <SgtCore/BusParser.h>

namespace SmartGridToolbox
{
   void SimBusParser::parse(const YAML::Node& nd, Simulation& data) const
   {
      SGT_DEBUG(debug() << "SimBus : parse." << std::endl);
      
      auto bus = parseSimBus(nd);

      assertFieldPresent(nd, "network_id");
      string netwId = nd["network_id"].as<std::string>();
      auto netw = sim.simComponent<SimNetwork>(netwId);
      netw->addNode(std::move(bus));
   }
}
