#include "SimGenericBranchParser.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/GenericBranch.h>
#include <SgtCore/GenericBranchParser.h>

#include <memory>

namespace SmartGridToolbox
{
   void SimGenericBranchParser::parse(const YAML::Node& nd, Simulation& sim) const
   {
      SGT_DEBUG(debug() << "SimGenericBranch : parse." << std::endl);

      auto gBranch = parseSimGenericBranch(nd);

      assertFieldPresent(nd, "network_id");
      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");
      
      string netwId = nd["network_id"].as<std::string>();
      std::string bus0Id = nd["bus_0_id"].as<std::string>();
      std::string bus1Id = nd["bus_1_id"].as<std::string>();
      
      auto netw = sim.simComponent<SimNetwork>(netwId);

      netw->addArc(std::move(gBranch), bus0Id, bus1Id);
   }
   
   std::unique_ptr<SimBranch> SimGenericBranchParser::parseSimGenericBranch(const YAML::Node& nd) const
   {
      static GenericBranchParser gbParser;
      return std::unique_ptr<SimBranch>(new SimBranch(gbParser.parseGenericBranch(nd)));
   }
}
