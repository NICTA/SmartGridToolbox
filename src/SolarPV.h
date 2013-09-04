#ifndef SOLAR_PV_DOT_H
#define SOLAR_PV_DOT_H

#include "Component.h"
#include "DCPowerSourceBase.h"
#include "Parser.h"

namespace SmartGridToolbox
{
   class Weather;

   class SolarPVParser : public ComponentParser
   {
      public:
         static constexpr const char * componentName()
         {
            return "solar_PV";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod) const override;

         virtual void postParse(const YAML::Node & nd, Model & mod) const override;
   };

   class SolarPV : public DCPowerSourceBase
   {
      /// @name My public member functions. 
      /// @{
      public:
         SolarPV(const std::string & name) :
            DCPowerSourceBase(name), weather_(nullptr), efficiency_(1.0), plane_({0.0, 1.0, 0.0})
         {}

         virtual double PDC() const override;

         void setWeather(const Weather & weather) {weather_ = &weather;}

         void setEfficiency(double efficiency) {efficiency_ = efficiency;}

         void setPlane(const Array<double, 3> & plane) {plane_ = plane;}

      public:
         const Weather * weather_;
         double efficiency_;
         Array<double, 3> plane_; // TODO : more than one plane?
   };
}

#endif // SOLAR_PV_DOT_H
