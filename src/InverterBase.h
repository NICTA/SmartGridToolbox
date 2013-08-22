#ifndef INVERTER_BASE_DOT_H
#define INVERTER_BASE_DOT_H

#include "ZipToGroundBase.h"

namespace SmartGridToolbox
{
   /// InverterBase: DC power to n-phase AC converter.
   class InverterBase : public ZipToGroundBase
   {
      /// @name Public overridden member functions from ZipToGroundBase.
      /// @{
      public:
         virtual const UblasVector<Complex> & getY() {return UblasVector<Complex>(getPhases().size(), czero);}
         virtual const UblasVector<Complex> & getI() {return UblasVector<Complex>(getPhases().size(), czero);}
         virtual const UblasVector<Complex> & getS() 
         {
            PDC = getDCPower();
            return polar(PDC * getEfficiency(PDC), getPowerFactorRadians());
         }
      /// @}
      
      /// @name My private member functions.
      /// @{
      private:
         virtual double getEfficiency(const double powerDC) const = 0;
         virtual double getDCPower() = 0;
         virtual double getPowerFactorRadians() = 0;
      /// @}
   };
}

#endif // INVERTER_BASE_DOT_H
