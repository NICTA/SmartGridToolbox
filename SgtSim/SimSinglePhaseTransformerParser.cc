#include "SimSinglePhaseTransformerParser.h"

#include "SimNetwork.h"

#include <SgtCore/SinglePhaseTransformer.h>
#include <SgtCore/SinglePhaseTransformerParser.h>

#include <memory>

namespace SmartGridToolbox
{
   void SimSinglePhaseTransformerParser::parse(const YAML::Node& nd, Simulation& sim) const
   {
      SGT_DEBUG(debug() << "SinglePhaseTransformer : parse." << std::endl);

      SinglePhaseTransformerParser transParser;
      auto trans = std::unique_ptr<SimSinglePhaseTransformer>(
            new SimSinglePhaseTransformer(transParser.parseSinglePhaseTransformer(nd)));
      
      assertFieldPresent(nd, "network_id");
      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");

      string netwId = nd["network_id"].as<std::string>();
      std::string bus0Id = nd["bus_0_id"].as<std::string>();
      std::string bus1Id = nd["bus_1_id"].as<std::string>();

      auto netw = sim.simComponent<SimNetwork>(netwId);

      netw->addArc(trans, bus0Id, bus1Id);
   }
}
