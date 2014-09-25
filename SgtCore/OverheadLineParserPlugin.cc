#include "OverheadLineParserPlugin.h"

#include "OverheadLine.h"
#include "Network.h"

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
      ublas::vector<double> lineResistivity = parser.expand<ublas::vector<double>>(nd["conductor_R_per_L"]);
      double earthResistivity = parser.expand<double>(nd["earth_resistivity"]);
      ublas::matrix<double> distMatrix = parser.expand<ublas::matrix<double>>(nd["distance_matrix"]);
      double freq = parser.expand<double>(nd["freq"]);

      std::unique_ptr<OverheadLine> ohl(new OverheadLine(id, phases0, phases1, length, nNeutral, lineResistivity,
               earthResistivity, distMatrix, freq));

      return ohl;
   }
}
