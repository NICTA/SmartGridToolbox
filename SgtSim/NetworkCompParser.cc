#include "NetworkCompParser.h"

#include "NetworkComp.h"

namespace SmartGridToolbox
{
   void NetworkCompParser::parse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "NetworkComp : parse." << std::endl);

      assertFieldPresent(nd, "name");

      string name = state.expandName(nd["name"].as<std::string>());

      auto ndFreq = nd["freq_Hz"];
      double freq = ndFreq ? ndFreq.as<double>() : 50.0;

      NetworkComp& comp = mod.newComponent<NetworkComp>(name, freq);
   }
}
