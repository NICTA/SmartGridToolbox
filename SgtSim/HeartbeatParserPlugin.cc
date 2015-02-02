#include "HeartbeatParserPlugin.h"

#include "Heartbeat.h"

#include <SgtSim/SimNetwork.h>
#include <SgtSim/Simulation.h>

namespace SmartGridToolbox
{
   void HeartbeatParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "dt");

      string id = parser.expand<std::string>(nd["id"]);
      Time dt = parser.expand<Time>(nd["dt"]);
      
      auto heartbeat = sim.newSimComponent<Heartbeat>(id, dt);
   }
}
