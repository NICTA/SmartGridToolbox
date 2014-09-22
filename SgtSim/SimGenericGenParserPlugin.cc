#include "SimGenericGenParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/Gen.h>
#include <SgtCore/GenericGenParserPlugin.h>

#include <memory>

namespace SmartGridToolbox
{
   void SimGenericGenParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const
   {
      GenericGenParserPlugin ggParser;
      auto gg = sim.newSimComponent<SimGen<GenericGen>>(ggParser.parseGenericGen(nd, state));

      assertFieldPresent(nd, "network_id");
      assertFieldPresent(nd, "bus_id");
      
      string netwId = state.expandName(nd["network_id"].as<std::string>());
      std::string busId = state.expandName(nd["bus_id"].as<std::string>());
      
      auto netw = sim.simComponent<SimNetwork>(netwId);

      netw->addGen(gg, busId);
   }
}
