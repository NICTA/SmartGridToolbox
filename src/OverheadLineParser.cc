#include "OverheadLineParser.h"

#include "OverheadLine.h"

namespace SmartGridToolbox
{
   void OverheadLineParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "OverheadLine : parse." << std::endl);

      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "bus_0");
      assertFieldPresent(nd, "bus_1");
      assertFieldPresent(nd, "phases_0");
      assertFieldPresent(nd, "phases_1");
      assertFieldPresent(nd, "length");
      assertFieldPresent(nd, "line_resistivity");
      assertFieldPresent(nd, "earth_resistivity");
      assertFieldPresent(nd, "distance_matrix");
      assertFieldPresent(nd, "freq");

      string name = state.expandName(nd["name"].as<std::string>());
      Phases phases0 = nd["phases_0"].as<Phases>();
      Phases phases1 = nd["phases_1"].as<Phases>();
      double length = nd["length"].as<double>();
      int nNeutral = nd["n_neutral"].as<int>();
      ublas::vector<double> lineResistivity = nd["line_resistivity"].as<ublas::vector<double>>();
      double earthResistivity = nd["earth_resistivity"].as<double>();
      ublas::matrix<double> distMatrix = nd["distance_matrix"].as<ublas::matrix<double>>();
      double freq = nd["freq"].as<double>();
      
      mod.newComponent<OverheadLine>(name, phases0, phases1, length, nNeutral, lineResistivity,
                                     earthResistivity, distMatrix, freq);
   }
}
