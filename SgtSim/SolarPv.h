#ifndef SOLAR_PV_DOT_H
#define SOLAR_PV_DOT_H

#include <SgtSim/DcPowerSource.h>
#include <SgtSim/Sun.h>

namespace Sgt
{
   class Weather;

   class SolarPv : public DcPowerSourceAbc
   {

      /// @name Static member functions:
      /// @{

      public:

         static const std::string& sComponentType()
         {
            static std::string result("component");
            return result;
         }
      
      /// @}
      
      /// @name Lifecycle:
      /// @{
      
      public:

         SolarPv(const std::string& id);
      
      /// @}

      /// @name ComponentInterface virtual overridden functions.
      /// @{
        
      public:

         virtual const std::string& componentType() const override
         {
            return sComponentType();
         }

         // virtual void print(std::ostream& os) const override; TODO

      /// @}

      /// @name Overridden member functions from SimComponent.
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

         void setWeather(std::shared_ptr<Weather> weather);

         void setEfficiency(double efficiency) {efficiency_ = efficiency; needsUpdate().trigger();}

         void setPlaneNormal(SphericalAngles planeNormal) {planeNormal_ = planeNormal; needsUpdate().trigger();}

         void setPlaneArea(double planeArea) {planeArea_ = planeArea; needsUpdate().trigger();}

         virtual double PDc() const override;

      /// @}
      
      private:
         std::shared_ptr<const Weather> weather_;
         double efficiency_;
         SphericalAngles planeNormal_; // TODO : more than one plane?
         double planeArea_; // TODO : more than one plane?
   };
}

#endif // SOLAR_PV_DOT_H
