#include "TimeSeriesZipParserPlugin.h"

#include "TimeSeriesZip.h"
#include "SimNetwork.h"
#include "Simulation.h"

namespace SmartGridToolbox
{
   void TimeSeriesZipParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases");
      assertFieldPresent(nd, "network_id");
      assertFieldPresent(nd, "bus_id");
      assertFieldPresent(nd, "time_series_id");
      assertFieldPresent(nd, "dt");

      string id = state.expandName(nd["id"].as<std::string>());
      Phases phases = nd["phases"].as<Phases>();
      string networkId = state.expandName(nd["network_id"].as<std::string>());
      string busId = state.expandName(nd["bus_id"].as<std::string>());
      string tsId = state.expandName(nd["time_series_id"].as<std::string>());
      Time dt = nd["dt"].as<Time>();
      
      auto series = sim.timeSeries<const TimeSeries<Time, ublas::vector<Complex>>>(tsId);
      auto network = sim.simComponent<SimNetwork>(networkId);
      auto tsZip = sim.newSimComponent<TimeSeriesZip>(id, phases, series, dt);
      network->addZip(tsZip, busId);
   }
}
