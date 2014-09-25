#include "SimGenericBranchParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/Branch.h>
#include <SgtCore/GenericBranchParserPlugin.h>

#include <memory>

namespace SmartGridToolbox
{
   void SimGenericBranchParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
   {
      GenericBranchParserPlugin gbParser;
      auto gb = sim.newSimComponent<SimGenericBranch>(*gbParser.parseGenericBranch(nd, parser));

      assertFieldPresent(nd, "network_id");
      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");
      
      string netwId = parser.expand<std::string>(nd["network_id"]);
      std::string bus0Id = parser.expand<std::string>(nd["bus_0_id"]);
      std::string bus1Id = parser.expand<std::string>(nd["bus_1_id"]);
      
      auto netw = sim.simComponent<SimNetwork>(netwId);

      netw->addArc(gb, bus0Id, bus1Id);
   }
}
