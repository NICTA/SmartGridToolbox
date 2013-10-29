#ifndef WEATHER_DOT_H
#define WEATHER_DOT_H

#include <smartgridtoolbox/Parser.h>
#include <smartgridtoolbox/RegularUpdateComponent.h>
#include <smartgridtoolbox/Sun.h>
#include <smartgridtoolbox/TimeSeries.h>

namespace SmartGridToolbox
{
   class WeatherParser : public ParserPlugin
   {
      public:
         static constexpr const char * pluginKey()
         {
            return "weather";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };

   struct SolarIrradiance
   {
      Array<double, 3> direct;
      double horizontalDiffuse;
   };

   class Weather : public RegularUpdateComponent
   {
      public:
         Weather(const std::string & name) :
            RegularUpdateComponent(name),
            latLong_(Greenwich),
            cloudCoverSeries_(nullptr)
         {
            setDt(posix_time::minutes(5));
         }

         virtual ~Weather() {delete cloudCoverSeries_;}

         void setLatLong(const LatLong & latLong) {latLong_ = latLong;}

         void acquireCloudCoverSeries(TimeSeries<Time, double> * newSeries)
         {
            std::swap(cloudCoverSeries_, newSeries);
         }

         double solarPower(SphericalAngles planeNormal, double planeArea) const;

         const SolarIrradiance & irradiance()
         {
            return irradiance_;
         }

      private:
         virtual void initializeState()
         {
            irradiance_ = unaveragedIrradiance(startTime());
         }

         virtual void updateState(Time t0, Time t1) override;

         SolarIrradiance unaveragedIrradiance(const Time & tm) const;

      private:
         LatLong latLong_;
         TimeSeries<Time, double> * cloudCoverSeries_;

         SolarIrradiance prevIrradiance_;
         SolarIrradiance irradiance_;
   };
}

#endif // WEATHER_DOT_H
