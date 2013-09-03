#include "SimpleBattery.h"
#include <iostream>
using namespace std;
namespace SmartGridToolbox
{
   void SimpleBattery::updateState(time_duration t0, time_duration t1)
   {
      charge_ += internalPower() * dSeconds(t1 - t0);
      if (charge_ < 0.0) charge_ = 0.0;
   }
}
