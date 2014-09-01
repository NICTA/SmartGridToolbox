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

      auto dgyTrans = parseSimDgyTransformer(nd);
      
      assertFieldPresent(nd, "network_id");
      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");

      string netwId = nd["network_id"].as<std::string>();
      std::string bus0Id = nd["bus_0_id"].as<std::string>();
      std::string bus1Id = nd["bus_1_id"].as<std::string>();

      auto netw = sim.simComponent<SimNetwork>(netwId);

      netw->addArc(std::move(ohl), bus0Id, bus1Id);
   }

   std::unique_ptr<SimBranch> SimDgyTransformerParser::parseSimDgyTransformer(const YAML::Node& nd) const
   {
      static SimDgyTransformerParser dgyTransParser;
      return std::unique_ptr<SimBranch>(new SimBranch(dgyTransParser.parseDgyTransformer(nd)));
   }
}
