#include "SimDgyTransformerParser.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/DgyTransformer.h>
#include <SgtCore/DgyTransformerParser.h>

#include <memory>

namespace SmartGridToolbox
{
   void SimDgyTransformerParser::parse(const YAML::Node& nd, Simulation& sim) const
   {
      DgyTransformerParser transParser;
      auto trans = sim.newSimComponent<SimBranch<DgyTransformer>>(transParser.parseDgyTransformer(nd));
      
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
