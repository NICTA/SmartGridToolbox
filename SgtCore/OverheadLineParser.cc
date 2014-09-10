#include "OverheadLineParser.h"

#include "OverheadLine.h"
#include "Network.h"

namespace SmartGridToolbox
{
   void OverheadLineParser::parse(const YAML::Node& nd, Network& netw) const
   {
      auto ohl = parseOverheadLine(nd);

      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");

      std::string bus0Id = nd["bus_0_id"].as<std::string>();
      std::string bus1Id = nd["bus_1_id"].as<std::string>();
      
      netw.addArc(std::move(ohl), bus0Id, bus1Id);
   }

   std::unique_ptr<OverheadLine> OverheadLineParser::parseOverheadLine(const YAML::Node& nd) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases_0");
      assertFieldPresent(nd, "phases_1");
      assertFieldPresent(nd, "length");
      assertFieldPresent(nd, "n_neutral");
      assertFieldPresent(nd, "line_resistivity");
      assertFieldPresent(nd, "earth_resistivity");
      assertFieldPresent(nd, "distance_matrix");
      assertFieldPresent(nd, "freq");

      string id = nd["id"].as<std::string>();
      Phases phases0 = nd["phases_0"].as<Phases>();
      Phases phases1 = nd["phases_1"].as<Phases>();
      double length = nd["length"].as<double>();
      int nNeutral = nd["n_neutral"].as<int>();
      ublas::vector<double> lineResistivity = nd["line_resistivity"].as<ublas::vector<double>>();
      double earthResistivity = nd["earth_resistivity"].as<double>();
      ublas::matrix<double> distMatrix = nd["distance_matrix"].as<ublas::matrix<double>>();
      double freq = nd["freq"].as<double>();

      std::unique_ptr<OverheadLine> ohl(new OverheadLine(id, phases0, phases1, length, nNeutral, lineResistivity,
               earthResistivity, distMatrix, freq));

      return ohl;
   }
}
