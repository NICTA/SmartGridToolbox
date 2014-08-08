#include "NetworkParser.h"

#include <SmartGridToolbox/Network.h>

namespace SmartGridToolbox
{
   void NetworkParser::parse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "Network : parse." << std::endl);

      assertFieldPresent(nd, "name");

      string name = state.expandName(nd["name"].as<std::string>());

      auto ndFreq = nd["freq_Hz"];
      double freq = ndFreq ? ndFreq.as<double>() : 50.0;

      Network& comp = mod.newComponent<Network>(name, freq);
   }
}
