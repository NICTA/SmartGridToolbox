#ifndef POWERFLOW_DOT_H
#define POWERFLOW_DOT_H

namespace SmartGridToolbox
{
   enum class BusType : int
   {
      SL = 0,
      PQ = 1,
      PV = 2
   };

   enum class Phases : int
   {
      A = 0,
      B = 1,
      C = 2
   };
}

#endif // POWERFLOW_DOT_H
