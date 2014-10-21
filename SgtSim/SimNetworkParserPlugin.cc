#include "SimNetworkParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

namespace SmartGridToolbox
{
   void SimNetworkParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "P_base");

      string id = parser.expand<std::string>(nd["id"]);
      double PBase = parser.expand<double>(nd["P_base"]);

      auto ndFreq = nd["freq_Hz"];

      std::unique_ptr<Network> nw = std::unique_ptr<Network>(new Network(id, PBase));
      if (ndFreq)
      {
         nw->setNomFreq(parser.expand<double>(ndFreq));
         nw->setFreq(nw->nomFreq());
      }
      sim.newSimComponent<SimNetwork>(std::move(nw));
   }
}
