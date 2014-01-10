#include "WeatherParser.h"

#include <SmartGridToolbox/Weather.h>

namespace SmartGridToolbox
{
   void WeatherParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "Weather : parse." << std::endl);

      assertFieldPresent(nd, "name");

      string name = state.expandName(nd["name"].as<std::string>());
      Weather & comp = mod.newComponent<Weather>(name);

      comp.setLatLong(mod.latLong());

      const auto & cloudNd = nd["cloud_cover"];
      if (cloudNd)
      {
         std::string name = cloudNd.as<std::string>();
         const TimeSeries<Time, double> * series = mod.timeSeries<TimeSeries<Time, double>>(name);
         if (series == nullptr)
         {
            error() << "Parsing weather: couldn't find time series " << name << std::endl;
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
