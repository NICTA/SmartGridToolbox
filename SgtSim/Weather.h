#ifndef WEATHER_DOT_H
#define WEATHER_DOT_H

#include <SgtSim/RegularUpdateComponent.h>
#include <SgtSim/Sun.h>
#include <SgtSim/TimeSeries.h>

namespace SmartGridToolbox
{
   struct SolarIrradiance
   {
      Array<double, 3> direct;
      double horizontalDiffuse;
   };

   class Weather : public RegularUpdateComponent
   {
      /// @name Overridden member functions from Component.
      /// @{
      
      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         virtual void updateState(Time t) override;
      
      /// @}

      /// @name My member functions.
      /// @{
     
      public:
         Weather(const std::string& name) :
            RegularUpdateComponent(name),
            latLong_(Greenwich),
            cloudCoverSeries_(nullptr),
            temperatureSeries_(nullptr)
         {
            setDt(posix_time::minutes(5));
         }

         void setLatLong(const LatLong& latLong) {latLong_ = latLong; needsUpdate().trigger();}

         const TimeSeries<Time, double>* temperatureSeries() const
         {
            return temperatureSeries_;
         }
         void setTemperatureSeries(const TimeSeries<Time, double>& series)
         {
            temperatureSeries_ = &series; needsUpdate().trigger();
         }
         double temperature() const
         {
            return temperatureSeries_->value(time());
         }

         const TimeSeries<Time, double>* cloudCoverSeries() const
         {
            return cloudCoverSeries_;
         }
         void setCloudCoverSeries(const TimeSeries<Time, double>& series)
         {
            cloudCoverSeries_ = &series; needsUpdate().trigger();
         }
         double cloudCover() const
         {
            return cloudCoverSeries_->value(time());
         }

         double solarPower(SphericalAngles planeNormal, double planeArea) const;

         const SolarIrradiance& irradiance()
         {
            return irradiance_;
         }

      protected:
         SolarIrradiance unaveragedIrradiance(const Time& tm) const;

      /// @}
      
      private:
         LatLong latLong_;
         const TimeSeries<Time, double>* temperatureSeries_;
         const TimeSeries<Time, double>* cloudCoverSeries_;

         SolarIrradiance prevIrradiance_;
         SolarIrradiance irradiance_;
   };
}

#endif // WEATHER_DOT_H
