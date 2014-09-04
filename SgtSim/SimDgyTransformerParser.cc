#include "SimDgyTransformerParser.h"

#include "SimNetwork.h"

#include <SgtCore/DgyTransformer.h>
#include <SgtCore/DgyTransformerParser.h>

#include <memory>

namespace SmartGridToolbox
{
   void SimDgyTransformerParser::parse(const YAML::Node& nd, Simulation& sim) const
   {
      SGT_DEBUG(debug() << "DgyTransformer : parse." << std::endl);

      DgyTransformerParser transParser;
      auto trans = std::unique_ptr<SimDgyTransformer>(new SimDgyTransformer(transParser.parseDgyTransformer(nd)));
      
      assertFieldPresent(nd, "network_id");
      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");

      string netwId = nd["network_id"].as<std::string>();
      std::string bus0Id = nd["bus_0_id"].as<std::string>();
      std::string bus1Id = nd["bus_1_id"].as<std::string>();

      auto netw = sim.simComponent<SimNetwork>(netwId);

      netw->addArc(std::move(trans), bus0Id, bus1Id);
   }
}
