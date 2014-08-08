#ifndef SOLAR_PV_DOT_H
#define SOLAR_PV_DOT_H

#include <SmartGridToolbox/Component.h>
#include <SmartGridToolbox/DcPowerSourceBase.h>
#include <SmartGridToolbox/Sun.h>

namespace SmartGridToolbox
{
   class Weather;

   class SolarPv : public DcPowerSourceBase
   {
      /// @name Overridden member functions from Component.
      /// @{

      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;
      
      /// @}

      /// @name My public member functions.
      /// @{
     
      public:
         SolarPv(const std::string& name);

         void setWeather(Weather& weather);

         void setEfficiency(double efficiency) {efficiency_ = efficiency; needsUpdate().trigger();}

         void setPlaneNormal(SphericalAngles planeNormal) {planeNormal_ = planeNormal; needsUpdate().trigger();}

         void setPlaneArea(double planeArea) {planeArea_ = planeArea; needsUpdate().trigger();}

         virtual double PDc() const override;

      /// @}
      
      private:
         const Weather* weather_;
         double efficiency_;
         SphericalAngles planeNormal_; // TODO : more than one plane?
         double planeArea_; // TODO : more than one plane?
   };
}

#endif // SOLAR_PV_DOT_H
