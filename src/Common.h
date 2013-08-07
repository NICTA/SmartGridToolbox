#ifndef COMMON_DOT_H
#define COMMON_DOT_H

#include <complex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

#ifdef DEBUG
#define SGT_DEBUG(x) x
#else
#define SGT_DEBUG(x)
#endif

namespace SmartGridToolbox
{
   inline std::ostream & message() {return std::cout <<  "MESSAGE:\t";}
   inline std::ostream & warning() {return std::cerr <<  "WARNING:\t";}
   inline std::ostream & error() {return std::cerr <<    "ERROR:  \t";}
   inline std::ostream & debug() {return std::cout <<    "DEBUG:  \t";}
   inline void abort() {std::cerr << "ABORTING." << std::endl; ::abort();}

   /// @name Constant dimension 2D array type.
   /// @{
   // Note transposition of NR and NC to obey standard matrix index order. 
   template <class T, size_t NR, size_t NC>
   using Array2D = std::array<std::array<T, NC>, NR>;
   /// @}

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

   std::ostream & operator<<(std::ostream & os, const Complex & c);

   Complex string2Complex(const std::string & s);

   std::string complex2String(const Complex & c);
   /// @}
  
   /// @name Linear algebra
   /// @{
   using UblasRange = boost::numeric::ublas::range;

   /// Dense vector
   template<typename T> using UblasVector = boost::numeric::ublas::vector<T>;
   template<typename T> using UblasVectorRange = boost::numeric::ublas::vector_range<UblasVector<T>>;

   /// Sparse compressed vector
   template<typename T> using UblasCVector = boost::numeric::ublas::compressed_vector<T>;
   template<typename T> using UblasCVectorRange = boost::numeric::ublas::vector_range<UblasCVector<T>>;

   /// Dense matrix 
   template<typename T> using UblasMatrix = boost::numeric::ublas::matrix<T>;
   template<typename T> using UblasMatrixRange = boost::numeric::ublas::matrix_range<UblasMatrix<T>>;

   /// Sparse compressed matrix, by rows.
   template<typename T> using UblasCMatrix = boost::numeric::ublas::compressed_matrix<T>;
   template<typename T> using UblasCMatrixRange = boost::numeric::ublas::matrix_range<UblasCMatrix<T>>;

   std::ostream & operator<<(std::ostream & os, const UblasVector<double> & v);
   std::ostream & operator<<(std::ostream & os, const UblasVector<Complex> & v);
   /// @}

   /// @name Time
   /// @{
   using boost::posix_time::ptime;
   using boost::posix_time::time_duration;
   using boost::posix_time::seconds;
   using boost::posix_time::minutes;
   using boost::posix_time::hours;
   using boost::posix_time::not_a_date_time;
   using boost::posix_time::neg_infin;
   using boost::posix_time::pos_infin;
   using boost::posix_time::time_from_string;
   using boost::gregorian::date;

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
