#include "Battery.h"

#include "InverterBase.h"

#include <iostream>

using namespace std;

namespace SmartGridToolbox
{
   void Battery::updateState(Time t)
   {
      double dt = time() == posix_time::neg_infin ? 0 : dSeconds(t - time());
      if (dt > 0)
      {
         charge_ += internalPower() * dSeconds(t - time());
         if (charge_ < 0.0) charge_ = 0.0;
      }
   }

   double Battery::PDc() const
   {
      double result = 0.0;
      if ((requestedPower_ > 0 && charge_ < maxCharge_) || (requestedPower_ < 0 && charge_ > 0))
      {
         result = requestedPower_ < 0
            ? std::max(requestedPower_, -maxDischargePower_)
            : std::min(requestedPower_, maxChargePower_);
      }
      return result;
   }

   double Battery::internalPower()
   {
      double P = PDc();
      return (P > 0 ? P * chargeEfficiency_ : P / dischargeEfficiency_);
   }
}
