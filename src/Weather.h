#ifndef WEATHER_DOT_H
#define WEATHER_DOT_H

#include "RegularUpdateComponent.h"
#include "Sun.h"

namespace SmartGridToolbox
{
   class Weather : public RegularUpdateComponent
   {
      public:
         Weather(const std::string & name, const LatLong & latLong) :
            RegularUpdateComponent(name),
            latLong{}
         {
            setDt(minutes(5));
         }

         void setLatLong(const LatLong & latLong) {latLong_ = latLong;}

         double solarPower(Array<double, 3> plane)
         {
            SphericalAnglesRadians sun = sunPos(utcTime(time()), latLong_);
            return SmartGridToolbox::sunPower(sun, plane);
         }

      private:
         LatLong latLong_;
   };
}

#endif // WEATHER_DOT_H
