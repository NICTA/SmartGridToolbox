#include "WeatherParser.h"

#include "Simulation.h"
#include "Weather.h"

namespace SmartGridToolbox
{
   void WeatherParser::parse(const YAML::Node& nd, Simulation& into) const
   {
      SGT_DEBUG(debug() << "Weather : parse." << std::endl);

      assertFieldPresent(nd, "id");

      string id = nd["id"].as<std::string>();
      auto weather = into.newSimComponent<Weather>(id);

      weather->setLatLong(into.latLong());

      const auto& temperatureNd = nd["temperature"];
      if (temperatureNd)
      {
         std::string id = temperatureNd.as<std::string>();
         auto series = into.timeSeries<TimeSeries<Time, double>>(id);
         if (series == nullptr)
         {
            error() << "Parsing weather: couldn't find time series " << id << std::endl;
            abort();
         }
         weather->setTemperatureSeries(*series);
      }

      const auto& cloudNd = nd["cloud_cover"];
      if (cloudNd)
      {
         std::string id = cloudNd.as<std::string>();
         auto series = into.timeSeries<TimeSeries<Time, double>>(id);
         if (series == nullptr)
         {
            error() << "Parsing weather: couldn't find time series " << id << std::endl;
            abort();
         }
         weather->setCloudCoverSeries(*series);
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
