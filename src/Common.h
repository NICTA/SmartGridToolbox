#ifndef COMMON_DOT_H
#define COMMON_DOT_H

#include <cmath>
#include <complex>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace SmartGridToolbox
{
   // Constants.
   const double pi = 3.141592653589793238462643383279502884;
   const double second = 1.0;
   const double minute = 60.0 * second;
   const double hour = 60.0 * minute;
   const double day = 24.0 * hour;
   const double week = 7 * day;
   const double J = 1.0;
   const double kJ = 1000.0 * J;
   const double W = J / second;
   const double kW = 1000.0 * W;
   const double kWh = kW * hour;
   const double A = 1.0; 
   const double C = A / second; 
   const double K = 1.0; 

   // Complex numbers.
   typedef std::complex<double> Complex;

   const Complex czero = Complex(0.0, 0.0);

   inline void setPolar(Complex & c, double M, double theta)
   {
      c.real(M * cos(theta)); c.imag(M * sin(theta)); 
   }

   inline Complex operator*(int i, const Complex & c) 
   {
      return Complex(i * c.real(), i * c.imag());
   }

   inline Complex operator*(const Complex & c, int i) 
   {
      return Complex(i * c.real(), i * c.imag());
   }

   // Time stuff.
   using namespace boost::posix_time;
   using namespace boost::gregorian;

   const ptime epoch(date(1970,1,1));

   // The following conversion functions allow lower level access to internal
   // representation of both time_durations and ptimes. This is often useful
   // e.g. for spline interpolation of a time series.
   inline double dSeconds(const time_duration & d) 
   {
      return double(d.ticks())/time_duration::ticks_per_second();
   }

   inline double dSeconds(const ptime & t)
   {
      return dSeconds(t - epoch);
   }
}

#endif // COMMON_DOT_H
