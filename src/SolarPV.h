#ifndef SOLAR_PV_DOT_H
#define SOLAR_PV_DOT_H

#include <SmartGridToolbox/Component.h>
#include <SmartGridToolbox/DCPowerSourceBase.h>
#include <SmartGridToolbox/Parser.h>
#include <SmartGridToolbox/Sun.h>

namespace SmartGridToolbox
{
   class Weather;

   class SolarPVParser : public ParserPlugin
   {
      public:
         static constexpr const char * pluginKey()
         {
            return "solar_PV";
         }

      public:
         virtual void parse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
         virtual void postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const override;
   };

   class SolarPV : public DCPowerSourceBase
   {
      /// @name My public member functions. 
      /// @{
      public:
         SolarPV(const std::string & name);

         virtual double PDC() const override;

         void setWeather(Weather & weather);

         void setEfficiency(double efficiency) {efficiency_ = efficiency;}

         void setPlaneNormal(SphericalAngles planeNormal) {planeNormal_ = planeNormal;}

         void setPlaneArea(double planeArea) {planeArea_ = planeArea;}

      public:
         const Weather * weather_;
         double efficiency_;
         SphericalAngles planeNormal_; // TODO : more than one plane?
         double planeArea_; // TODO : more than one plane?
   };
}

#endif // SOLAR_PV_DOT_H
