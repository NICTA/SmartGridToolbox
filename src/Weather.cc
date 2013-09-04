#include "Weather.h"
#include "Model.h"
#include "TimeSeries.h"

namespace SmartGridToolbox
{
   void WeatherParser::parse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "Weather : parse." << std::endl);

      assertFieldPresent(nd, "name");

      const std::string nameStr = nd["name"].as<std::string>();
      Weather & comp = mod.newComponent<Weather>(nameStr);

      const auto & cloudNd = nd["cloud_cover"];
      if (cloudNd)
      {
         const auto & constNd = cloudNd["constant"];
         if (constNd)
         {
            comp.takeCloudCoverSeries(new ConstTimeSeries<Time, double>(constNd.as<double>()));  
         }
         else
         {
            error() << "Weather : cloud_cover keyword is present, but no value specified." << std::endl;
            abort();
         }
      }
      else
      {
         comp.takeCloudCoverSeries(new ConstTimeSeries<Time, double>(0.0));
      }
   }

   double Weather::solarPower(Array<double, 3> plane)
   {
      // Possibly dodgy model.
      // Assume transmitted and diffuse components vary linearly with the cloud cover coefficient.
      // Values taken haphazardly from "the web" - http://www.powerfromthesun.net/Book/chapter02/chapter02.html.

      const double maxTransmit = 0.83;
      const double minDiffuse = 0.05;

      const double minTransmit = 0.33;
      const double maxDiffuse = 0.26;

      double cloudCover = cloudCoverSeries_->value(time());

      double transmit = maxTransmit - cloudCover * (maxTransmit - minTransmit);
      double diffuse = minDiffuse + cloudCover * (maxDiffuse - minDiffuse);

      assert(cloudCover >= 0 && cloudCover <= 1);

      SphericalAnglesRadians sun = sunPos(utcTime(time()), latLong_);
      double pow = transmit * SmartGridToolbox::solarPowerInVacuo(sun, plane) + diffuse * solarIrradianceInVacuo();

      return pow;
   }
}
