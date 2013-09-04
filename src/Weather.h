#ifndef WEATHER_DOT_H
#define WEATHER_DOT_H

#include "RegularUpdateComponent.h"
#include "Sun.h"
#include "TimeSeries.h"

namespace SmartGridToolbox
{
   class Weather : public RegularUpdateComponent
   {
      public:
         Weather(const std::string & name) :
            RegularUpdateComponent(name),
            latLong_(Greenwich),
            cloudCoverSeries_(nullptr)
         {
            setDt(minutes(5));
         }

         void setLatLong(const LatLong & latLong) {latLong_ = latLong;}

         void takeCloudCoverSeries(TimeSeries<Time, double> * newSeries)
         {
            std::swap(cloudCoverSeries_, newSeries);
         }

         double solarPower(Array<double, 3> plane);

      private:
         LatLong latLong_;
         TimeSeries<Time, double> * cloudCoverSeries_;
   };
}

#endif // WEATHER_DOT_H
