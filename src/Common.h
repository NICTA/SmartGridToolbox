#ifndef COMMON_DOT_H
#define COMMON_DOT_H

#include <cmath>

namespace SmartGridToolbox
{
   typedef long int Timestamp;

   const double gTimestep = 0.001; // 1 ms timestep, 20 per 50 hz cycle.

   inline double dseconds(const Timestamp t)
   {
      return t * gTimestep;
   }

   inline long int iseconds(const Timestamp t)
   {
      return round(t * gTimestep);
   }
}

#endif // COMMON_DOT_H
