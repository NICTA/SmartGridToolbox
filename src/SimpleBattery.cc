#include "SimpleBattery.h"
#include <iostream>
using namespace std;
namespace SmartGridToolbox
{
   void SimpleBattery::updateState(ptime t)
   {
      ptime t_prev = getTimestamp();
      charge_ += getInternalPower() * dSeconds(t - t_prev);
      if (charge_ < 0.0) charge_ = 0.0;
   }
}
