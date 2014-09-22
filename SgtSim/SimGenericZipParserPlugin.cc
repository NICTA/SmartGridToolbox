#include "SimGenericZipParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/Zip.h>
#include <SgtCore/GenericZipParserPlugin.h>

#include <memory>

namespace SmartGridToolbox
{
   void SimGenericZipParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const
   {
      GenericZipParserPlugin gzParser;
      auto gz = sim.newSimComponent<SimZip<GenericZip>>(gzParser.parseGenericZip(nd, state));

      assertFieldPresent(nd, "network_id");
      assertFieldPresent(nd, "bus_id");
      
      string netwId = state.expandName(nd["network_id"].as<std::string>());
      std::string busId = state.expandName(nd["bus_id"].as<std::string>());
      
      auto netw = sim.simComponent<SimNetwork>(netwId);

      netw->addZip(gz, busId);
   }
}
