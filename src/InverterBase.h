#ifndef INVERTER_BASE_DOT_H
#define INVERTER_BASE_DOT_H

#include "ZipToGroundBase.h"

namespace SmartGridToolbox
{
   class DCPowerSourceBase;

   /// InverterBase: DC power to n-phase AC converter.
   class InverterBase : public ZipToGroundBase
   {
      /// @name Private overridden functions: from Component.
      /// @{
      private:
         virtual void initializeState(ptime t) override
         {
            updateState(t, t);
         }

         virtual void updateState(ptime t0, ptime t1) override;
      /// @}

      /// @name Public overridden member functions from ZipToGroundBase.
      /// @{
      public:
         virtual const UblasVector<Complex> & getY() {return UblasVector<Complex>(getPhases().size(), czero);}
         virtual const UblasVector<Complex> & getI() {return UblasVector<Complex>(getPhases().size(), czero);}
         virtual const UblasVector<Complex> & getS(); 
      /// @}
      
      /// @name My public member functions. 
      /// @{
      public:
         InverterBase(const std::string & name) : Component(name) {}

         void addDCPowerSource(const DCPowerSourceBase & source);
      /// @}
      
      /// @name My private member functions.
      /// @{
      private:
         virtual double getEfficiency(const double powerDC) const = 0;
         virtual double getDCPower() = 0;
         virtual double getPowerFactorRadians() = 0;
      /// @}
      
      /// @name My private member variables.
      /// @{
      private:
         std::vector<const DCPowerSourceBase *> sources_;    ///< My DC power sources.
         double PDC_;
      /// @}
   };
}

#endif // INVERTER_BASE_DOT_H
