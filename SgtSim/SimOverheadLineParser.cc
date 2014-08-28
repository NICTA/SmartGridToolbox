#include "SimOverheadLineParser.h"

#include "SimBus.h"
#include "SimOverheadLine.h"
#include "SimNetwork.h"

#include <SgtCore/OverheadLineParser.h>

namespace SmartGridToolbox
{
   void SimOverheadLineParser::parse(const YAML::Node& nd, Simulation& sim) const
   {
      SGT_DEBUG(debug() << "OverheadLine : parse." << std::endl);

      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");
      assertFieldPresent(nd, "phases_0");
      assertFieldPresent(nd, "phases_1");
      assertFieldPresent(nd, "length");
      assertFieldPresent(nd, "n_neutral");
      assertFieldPresent(nd, "line_resistivity");
      assertFieldPresent(nd, "earth_resistivity");
      assertFieldPresent(nd, "distance_matrix");
      assertFieldPresent(nd, "freq");


      string id = nd["id"].as<std::string>();
      string netwId = nd["network_id"].as<std::string>();
      std::string bus0Id = nd["bus_0_id"].as<std::string>();
      std::string bus1Id = nd["bus_1_id"].as<std::string>();
      Phases phases0 = nd["phases_0"].as<Phases>();
      Phases phases1 = nd["phases_1"].as<Phases>();
      double length = nd["length"].as<double>();
      int nNeutral = nd["n_neutral"].as<int>();
      ublas::vector<double> lineResistivity = nd["line_resistivity"].as<ublas::vector<double>>();
      double earthResistivity = nd["earth_resistivity"].as<double>();
      ublas::matrix<double> distMatrix = nd["distance_matrix"].as<ublas::matrix<double>>();
      double freq = nd["freq"].as<double>();

      std::unique_ptr<SimOverheadLine> ohl(new SimOverheadLine(id, phases0, phases1, length, nNeutral,
               lineResistivity, earthResistivity, distMatrix, freq));
      
      auto netw = sim.simComponent<SimNetwork>(netwId);
      std::shared_ptr<Bus> bus0 = netw->bus(bus0Id);
      assert(bus0 != nullptr);
      std::shared_ptr<Bus> bus1 = netw->bus(bus1Id);
      assert(bus1 != nullptr);
      ohl->setBus0(bus0);
      ohl->setBus1(bus1);
      
      netw->addBranch(std::move(ohl));
   }
}
