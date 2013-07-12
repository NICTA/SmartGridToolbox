#ifndef COMMON_DOT_H
#define COMMON_DOT_H

#include <cmath>
#include <complex>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>

namespace SmartGridToolbox
{
   /// @name Constant dimension 2D array type.
   /// @{
   // Note transposition of NR and NC to obey standard matrix index order. 
   template <class T, size_t NR, size_t NC>
   using Array2D = std::array<std::array<T, NC>, NR>;
   /// @}

   namespace ublas = boost::numeric::ublas;

   /// @name Complex numbers
   /// @{
   typedef std::complex<double> Complex;

   inline Complex Polar(double m, double theta)
   {
      // Note the following will use RVO in C++11, no unneeded temporaries.
      return Complex(m * cos(theta), m * sin(theta));
   }

   inline Complex operator*(int i, const Complex & c) 
   {
      return Complex(i * c.real(), i * c.imag());
   }

   inline Complex operator*(const Complex & c, int i) 
   {
      return Complex(i * c.real(), i * c.imag());
   }
   /// @}

   /// @name Time
   /// @{
   namespace posix_time = boost::posix_time;
   namespace gregorian = boost::gregorian;

   const posix_time::ptime epoch(gregorian::date(1970,1,1));

   // The following conversion functions allow lower level access to internal
   // representation of both time_durations and ptimes. This is often useful
   // e.g. for spline interpolation of a time series.
   inline double dSeconds(const posix_time::time_duration & d) 
   {
      return double(d.ticks())/posix_time::time_duration::ticks_per_second();
   }

   inline double dSeconds(const posix_time::ptime & t)
   {
      return dSeconds(t - epoch);
   }
   /// @}

   /// @name Constants
   /// @{
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
   const Complex czero = Complex(0.0, 0.0);
   /// @}
}

#endif // COMMON_DOT_H
