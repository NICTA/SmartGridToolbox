#include <SmartGridToolbox/SimpleBattery.h>
#include <SmartGridToolbox/InverterBase.h>
#include <iostream>

using namespace std;

namespace SmartGridToolbox
{
   void SimpleBattery::updateState(Time t0, Time t1)
   {
      double dt = t0 == posix_time::neg_infin ? 0 : dSeconds(t1 - t0);
      if (dt > 0)
      {
         charge_ += internalPower()*dSeconds(t1 - t0);
         if (charge_ < 0.0) charge_ = 0.0;
      }
   }

   double SimpleBattery::PDc() const
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
   
   double SimpleBattery::internalPower()
   {
      double P = PDc();
      return (P > 0 ? P*chargeEfficiency_ : P/dischargeEfficiency_);
   }
}
