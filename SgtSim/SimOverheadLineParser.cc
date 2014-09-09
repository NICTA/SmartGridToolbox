#include "SimOverheadLineParser.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/OverheadLine.h>
#include <SgtCore/OverheadLineParser.h>

#include <memory>

namespace SmartGridToolbox
{
   void SimOverheadLineParser::parse(const YAML::Node& nd, Simulation& sim) const
   {
      SGT_DEBUG(debug() << "OverheadLine : parse." << std::endl);
      
      OverheadLineParser ohlParser;
      auto ohl = std::make_shared<SimBranch<OverheadLine>>(ohlParser.parseOverheadLine(nd));

      assertFieldPresent(nd, "network_id");
      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");

      string netwId = nd["network_id"].as<std::string>();
      std::string bus0Id = nd["bus_0_id"].as<std::string>();
      std::string bus1Id = nd["bus_1_id"].as<std::string>();

      auto netw = sim.simComponent<SimNetwork>(netwId);

      netw->addArc(ohl, bus0Id, bus1Id);
   }
}
