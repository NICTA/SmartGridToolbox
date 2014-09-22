#include "SimNetworkParserPlugin.h"

#include "SimNetwork.h"
#include "Simulation.h"

namespace SmartGridToolbox
{
   void SimNetworkParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "P_base");

      string id = state.expandName(nd["id"].as<std::string>());
      double PBase = nd["P_base"].as<double>();

      auto ndFreq = nd["freq_Hz"];

      std::unique_ptr<Network> nw = std::unique_ptr<Network>(new Network(id, PBase));
      if (ndFreq)
      {
         nw->setFreq(ndFreq.as<double>());
      }
      sim.newSimComponent<SimNetwork>(std::move(nw));
   }
}
