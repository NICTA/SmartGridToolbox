#include "OverheadLineParserPlugin.h"

#include "OverheadLine.h"
#include "Network.h"
#include "YamlSupport.h"

namespace SmartGridToolbox
{
   void OverheadLineParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
   {
      auto ohl = parseOverheadLine(nd, parser);

      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");

      std::string bus0Id = parser.expand<std::string>(nd["bus_0_id"]);
      std::string bus1Id = parser.expand<std::string>(nd["bus_1_id"]);
      
      netw.addArc(std::move(ohl), bus0Id, bus1Id);
   }

   std::unique_ptr<OverheadLine> OverheadLineParserPlugin::parseOverheadLine(const YAML::Node& nd,
         const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases_0");
      assertFieldPresent(nd, "phases_1");
      assertFieldPresent(nd, "length");
      assertFieldPresent(nd, "n_neutral");
      assertFieldPresent(nd, "conductor_R_per_L");
      assertFieldPresent(nd, "earth_resistivity");
      assertFieldPresent(nd, "distance_matrix");
      assertFieldPresent(nd, "freq");

      string id = parser.expand<std::string>(nd["id"]);
      Phases phases0 = parser.expand<Phases>(nd["phases_0"]);
      Phases phases1 = parser.expand<Phases>(nd["phases_1"]);
      double length = parser.expand<double>(nd["length"]);
      int nNeutral = parser.expand<int>(nd["n_neutral"]);
      arma::Col<double> lineResistivity = parser.expand<arma::Col<double>>(nd["conductor_R_per_L"]);
      double earthResistivity = parser.expand<double>(nd["earth_resistivity"]);
      arma::Mat<double> distMatrix = parser.expand<arma::Mat<double>>(nd["distance_matrix"]);
      double freq = parser.expand<double>(nd["freq"]);

      std::unique_ptr<OverheadLine> ohl(new OverheadLine(id, phases0, phases1, length, nNeutral, lineResistivity,
               earthResistivity, distMatrix, freq));

      return ohl;
   }
}
