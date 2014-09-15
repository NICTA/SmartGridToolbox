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
      /// @name Overridden member functions from SimComponent.
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
         Weather(const std::string& id) :
            RegularUpdateComponent(id),
            latLong_(Greenwich),
            cloudCoverSeries_(nullptr),
            temperatureSeries_(nullptr)
         {
            setDt(posix_time::minutes(5));
         }

         static constexpr const char* sComponentType()
         {
            return "weather";
         }

         virtual const char* componentType() const override
         {
            return sComponentType();
         }

         void setLatLong(const LatLong& latLong) {latLong_ = latLong; needsUpdate().trigger();}

         std::shared_ptr<const TimeSeries<Time, double>> temperatureSeries() const
         {
            return temperatureSeries_;
         }
         void setTemperatureSeries(std::shared_ptr<const TimeSeries<Time, double>> series)
         {
            temperatureSeries_ = series; needsUpdate().trigger();
         }
         double temperature() const
         {
            return temperatureSeries_->value(time());
         }

         std::shared_ptr<const TimeSeries<Time, double>> cloudCoverSeries() const
         {
            return cloudCoverSeries_;
         }
         void setCloudCoverSeries(std::shared_ptr<const TimeSeries<Time, double>> series)
         {
            cloudCoverSeries_ = series; needsUpdate().trigger();
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
         std::shared_ptr<const TimeSeries<Time, double>> temperatureSeries_;
         std::shared_ptr<const TimeSeries<Time, double>> cloudCoverSeries_;

         SolarIrradiance prevIrradiance_;
         SolarIrradiance irradiance_;
   };
}

#endif // WEATHER_DOT_H
