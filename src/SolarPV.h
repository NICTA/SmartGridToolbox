#ifndef SOLAR_PV_DOT_H
#define SOLAR_PV_DOT_H

#include "Component.h"
#include "Sun.h"

namespace SmartGridToolbox
{
   class SolarPV : public DCPowerSourceBase
   {
      /// @name My public member functions. 
      /// @{
      public:
         DCPowerSourceBase(const std::string & name) : Component(name), weather(nullptr) {}

         virtual double PDC() const override;

         void setWeather(const Weather & weather) {weather_ = &weather;}

      public:
            const Weather * weather_;
   };
}

#endif // SOLAR_PV_DOT_H
