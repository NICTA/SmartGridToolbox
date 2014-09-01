#include "WeatherParser.h"

#include "Weather.h"

namespace SmartGridToolbox
{
   void WeatherParser::parse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "Weather : parse." << std::endl);

      assertFieldPresent(nd, "id");

      string id = state.expandName(nd["id"].as<std::string>());
      Weather& comp = mod.newComponent<Weather>(id);

      comp.setLatLong(mod.latLong());

      const auto& temperatureNd = nd["temperature"];
      if (temperatureNd)
      {
         std::string id = temperatureNd.as<std::string>();
         const TimeSeries<Time, double> * series = mod.timeSeries<TimeSeries<Time, double>>(id);
         if (series == nullptr)
         {
            error() << "Parsing weather: couldn't find time series " << id << std::endl;
            abort();
         }
         comp.setTemperatureSeries(*series);
      }

      const auto& cloudNd = nd["cloud_cover"];
      if (cloudNd)
      {
         std::string id = cloudNd.as<std::string>();
         const TimeSeries<Time, double> * series = mod.timeSeries<TimeSeries<Time, double>>(id);
         if (series == nullptr)
         {
            error() << "Parsing weather: couldn't find time series " << id << std::endl;
            abort();
         }
         comp.setCloudCoverSeries(*series);
      }

      if (nd["dt"])
      {
         comp.setDt(nd["dt"].as<Time>());
      }
      else
      {
         comp.setDt(posix_time::minutes(5.0));
      }
   }
}
