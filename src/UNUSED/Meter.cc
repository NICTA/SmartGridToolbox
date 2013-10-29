#include "Meter.h"

namespace SmartGridToolbox
{
   void Meter::updateState(Time t0, Time t1)
   {
      P_ = Complex(0.0, 0.0);
      for (Component * comp : comps_)
      {
         comp->ensureAtTime(t1);
         const Load & ld = dynamic_cast<const Load &>(*comp);
         P_ += ld.power();
      }
   }
}
