#include "SimpleBattery.h"

namespace SmartGridToolbox
{
   void SimpleBattery::advance(ptime t)
   {
      ptime t_prev = getTimestamp();
      double pow = requestedPower_;
      if (pow > maxChargePower_) pow = maxChargePower_;
      if (pow < -maxDischargePower_) pow = -maxDischargePower_;
      if (pow < 0) pow /= dischargeEfficiency_;
      if (pow > 0) pow *= chargeEfficiency_;
      charge_ += pow * seconds(t - t_prev);
      if (charge_ < 0.0) charge_ = 0.0;
   }
}
