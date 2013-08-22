#include "InverterBase.h"

namespace SmartGridToolbox
{
   void InverterBase::updateState(ptime t0, ptime t1)
   {
      PDC_ = 0.0;
      for (const DCPowerSourceBase * source : sources_)
      {
         PDC_ += source->getPDC();
      }
   }

   const UblasVector<Complex> & InverterBase::getS()
   {
      double PDC = getDCPower();
      return UblasVector<Complex>(getPhases.size(), polar(PDC * getEfficiency(PDC), getPowerFactorRadians()));
   }
}
