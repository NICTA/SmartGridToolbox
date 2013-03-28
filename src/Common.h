#ifndef COMMON_DOT_H
#define COMMON_DOT_H

#include <cmath>

namespace SmartGridToolbox
{
   typedef long int Timestamp;

   const double gTimestep = 0.001; // 1 ms timestep, 20 per 50 hz cycle.
   const double gSecond = 1 / gTimestep;
   const double gMinute = 60 * gSecond;
   const double gHour = 60 * gMinute;
   const double gDay = 24 * gHour;
   const double gWeek = 7 * gDay;

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
