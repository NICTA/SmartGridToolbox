#include "SimGenericZipParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

#include <SgtCore/Zip.h>
#include <SgtCore/GenericZipParserPlugin.h>

#include <memory>

namespace SmartGridToolbox
{
   void SimGenericZipParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
   {
      GenericZipParserPlugin gzParser;
      auto gz = sim.newSimComponent<SimGenericZip>(*gzParser.parseGenericZip(nd, parser));

      assertFieldPresent(nd, "network_id");
      assertFieldPresent(nd, "bus_id");
      
      string netwId = parser.expand<std::string>(nd["network_id"]);
      std::string busId = parser.expand<std::string>(nd["bus_id"]);
      
      auto netw = sim.simComponent<SimNetwork>(netwId);

      netw->addZip(gz, busId);
   }
}
