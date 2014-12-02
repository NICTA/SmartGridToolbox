#include "DgyTransformerParserPlugin.h"

#include "DgyTransformer.h"
#include "Network.h"
#include "YamlSupport.h"

namespace SmartGridToolbox
{
   void DgyTransformerParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
   {
      auto trans = parseDgyTransformer(nd, parser);

      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");

      std::string bus0Id = parser.expand<std::string>(nd["bus_0_id"]);
      std::string bus1Id = parser.expand<std::string>(nd["bus_1_id"]);

      netw.addArc(std::move(trans), bus0Id, bus1Id);
   }

   std::unique_ptr<DgyTransformer> DgyTransformerParserPlugin::parseDgyTransformer(const YAML::Node& nd,
         const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "nom_V_ratio_DY");
      assertFieldPresent(nd, "leakage_impedance");

      const std::string id = parser.expand<std::string>(nd["id"]);
      Complex nomVRatioDY = parser.expand<Complex>(nd["nom_V_ratio_DY"]);
      auto ndOffNomRatioDY = nd["off_nom_ratio_DY"];
      Complex offNomRatioDY = ndOffNomRatioDY ? parser.expand<Complex>(nd["off_nom_ratio_DY"]) : 1.0;

      Complex ZL = parser.expand<Complex>(nd["leakage_impedance"]);

      std::unique_ptr<DgyTransformer> trans(new DgyTransformer(id, nomVRatioDY, offNomRatioDY, ZL));

      return trans;
   }
}
