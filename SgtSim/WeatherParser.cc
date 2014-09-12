#include "WeatherParser.h"

#include "Simulation.h"
#include "Weather.h"

namespace SmartGridToolbox
{
   void WeatherParser::parse(const YAML::Node& nd, Simulation& sim) const
   {
      assertFieldPresent(nd, "id");

      string id = nd["id"].as<std::string>();
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
         weather->setDt(nd["dt"].as<Time>());
      }
      else
      {
         weather->setDt(posix_time::minutes(5.0));
      }
   }
}
