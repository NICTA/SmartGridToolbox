#include "WeatherParserPlugin.h"

#include "Simulation.h"
#include "Weather.h"

namespace SmartGridToolbox
{
   void WeatherParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");

      string id = parser.expand<std::string>(nd["id"]);
      auto weather = sim.newSimComponent<Weather>(id);

      weather->setLatLong(sim.latLong());

      const auto& temperatureNd = nd["temperature"];
      if (temperatureNd)
      {
         std::string id = temperatureNd.as<std::string>();
         auto series = sim.timeSeries<TimeSeries<Time, double>>(id);
         if (series == nullptr)
         {
            Log().fatal() << "Parsing weather: couldn't find time series " << id << std::endl;
         }
         weather->setTemperatureSeries(series);
      }

      const auto& cloudNd = nd["cloud_cover"];
      if (cloudNd)
      {
         std::string id = cloudNd.as<std::string>();
         auto series = sim.timeSeries<TimeSeries<Time, double>>(id);
         if (series == nullptr)
         {
            Log().fatal() << "Parsing weather: couldn't find time series " << id << std::endl;
         }
         weather->setCloudCoverSeries(series);
      }

      if (nd["dt"])
      {
         weather->setDt(parser.expand<Time>(nd["dt"]));
      }
      else
      {
         weather->setDt(posix_time::minutes(5.0));
      }
   }
}
