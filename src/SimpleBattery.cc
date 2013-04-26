#include "SimpleBattery.h"

namespace SmartGridToolbox
{
   void SimpleBattery::advance(ptime t)
   {
      ptime t_prev = getTimestamp();
      charge_ += getInternalPower() * dSeconds(t - t_prev);
      if (charge_ < 0.0) charge_ = 0.0;
   }
}
