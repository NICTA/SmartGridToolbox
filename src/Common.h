#ifndef COMMON_DOT_H
#define COMMON_DOT_H

#include <cmath>
#include <complex>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace SmartGridToolbox
{
   typedef std::complex<double> Complex;

   using namespace boost::posix_time;
   using namespace boost::gregorian;
   inline double seconds(const time_duration & d) 
   {
      return d.ticks()/time_duration::ticks_per_second();
   }

   const double s = 1.0;
   const double minute = 60.0 * s;
   const double hour = 60.0 * minute;
   const double day = 24.0 * hour;
   const double week = 7 * day;
   const double J = 1.0;
   const double kJ = 1000.0 * J;
   const double W = J / s;
   const double kW = 1000.0 * W;
   const double kWh = kW * hour; 
   const double A = 1.0; 
   const double C = A/s; 
}

#endif // COMMON_DOT_H
