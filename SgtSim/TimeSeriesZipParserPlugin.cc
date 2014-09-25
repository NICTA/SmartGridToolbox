#include "TimeSeriesZipParserPlugin.h"

#include "TimeSeriesZip.h"
#include "SimNetwork.h"
#include "Simulation.h"

namespace SmartGridToolbox
{
   void TimeSeriesZipParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases");
      assertFieldPresent(nd, "network_id");
      assertFieldPresent(nd, "bus_id");
      assertFieldPresent(nd, "time_series_id");
      assertFieldPresent(nd, "dt");

      string id = parser.expand<std::string>(nd["id"]);
      Phases phases = parser.expand<Phases>(nd["phases"]);
      string networkId = parser.expand<std::string>(nd["network_id"]);
      string busId = parser.expand<std::string>(nd["bus_id"]);
      string tsId = parser.expand<std::string>(nd["time_series_id"]);
      Time dt = parser.expand<Time>(nd["dt"]);
      
      auto series = sim.timeSeries<const TimeSeries<Time, ublas::vector<Complex>>>(tsId);
      auto network = sim.simComponent<SimNetwork>(networkId);
      auto tsZip = sim.newSimComponent<TimeSeriesZip>(id, phases, series, dt);
      network->addZip(tsZip, busId);
   }
}
