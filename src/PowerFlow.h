#ifndef POWERFLOW_DOT_H
#define POWERFLOW_DOT_H

namespace SmartGridToolbox
{
   enum class BusType : int
   {
      PQ = 0,
      PV = 1,
      SL = 2
   };

   enum class Phases : int
   {
      A = 0,
      B = 1,
      C = 2
   };
}

#endif // POWERFLOW_DOT_H
