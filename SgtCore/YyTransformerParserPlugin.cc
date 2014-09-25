#include "YyTransformerParserPlugin.h"

#include "YyTransformer.h"
#include "Network.h"

namespace SmartGridToolbox
{
   void YyTransformerParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
   {
      auto trans = parseYyTransformer(nd, parser);

      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");

      std::string bus0Id = parser.expand<std::string>(nd["bus_0_id"]);
      std::string bus1Id = parser.expand<std::string>(nd["bus_1_id"]);

      netw.addArc(std::move(trans), bus0Id, bus1Id);
   }

   std::unique_ptr<YyTransformer> YyTransformerParserPlugin::parseYyTransformer(const YAML::Node& nd,
         const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "complex_turns_ratio_01");
      assertFieldPresent(nd, "leakage_impedance");

      const std::string id = parser.expand<std::string>(nd["id"]);
      Complex a = parser.expand<Complex>(nd["complex_turns_ratio_01"]);
      Complex ZL = parser.expand<Complex>(nd["leakage_impedance"]);
      auto ndYm = nd["magnetizing_admittance"];
      Complex YM = ndYm ? ndYm.as<Complex>() : czero;

      std::unique_ptr<YyTransformer> trans(new YyTransformer(id, a, ZL, YM));

      return trans;
   }
}
