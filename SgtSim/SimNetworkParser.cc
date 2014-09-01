#include "SimNetworkParser.h"

#include "SimNetwork.h"

namespace SmartGridToolbox
{
   void SimNetworkParser::parse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "SimNetwork : parse." << std::endl);

      assertFieldPresent(nd, "id");

      string id = state.expandName(nd["id"].as<std::string>());

      auto ndFreq = nd["freq_Hz"];
      double freq = ndFreq ? ndFreq.as<double>() : 50.0;

      SimNetwork& comp = mod.newComponent<SimNetwork>(id, freq);
   }
}
