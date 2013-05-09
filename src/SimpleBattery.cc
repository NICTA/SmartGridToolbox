#include "SimpleBattery.h"
#include <iostream>
using namespace std;
namespace SmartGridToolbox
{
   void SimpleBattery::updateState(ptime t0, ptime t1)
   {
      charge_ += getInternalPower() * dSeconds(t1 - t0);
      if (charge_ < 0.0) charge_ = 0.0;
   }
}
