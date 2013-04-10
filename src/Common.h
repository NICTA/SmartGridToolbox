#ifndef COMMON_DOT_H
#define COMMON_DOT_H

#include <cmath>
#include <complex>

namespace SmartGridToolbox
{
   typedef long long int Timestamp;

   const double gTimestep = 1.0e-6; // 1 us timestep, overkill for 50 Hz!
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

   typedef std::complex<double> Complex;
}

#endif // COMMON_DOT_H
