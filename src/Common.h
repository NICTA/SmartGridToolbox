#ifndef COMMON_DOT_H
#define COMMON_DOT_H

#include <complex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/local_time/local_time.hpp>
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

   /// @name Constant dimension 1D array type.
   /// @{
   template <class T, size_t N> using Array = std::array<T, N>; // Just std::array but rename for nice consistency.
   /// @}

   /// @name Constant dimension 2D array type.
   /// @{
   // Note transposition of NR and NC to obey standard matrix index order. 
   template <class T, size_t NR, size_t NC> using Array2D = std::array<std::array<T, NC>, NR>;
   /// @}

   /// @name Complex numbers
   /// @{
   typedef std::complex<double> Complex;

   inline Complex polar(double m, double theta) // theta is radians.
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

   template<typename VE> using UblasVectorExpression = boost::numeric::ublas::vector_expression<VE>;
   template<typename ME> using UblasMatrixExpression = boost::numeric::ublas::matrix_expression<ME>;

   template<typename VE> std::ostream & operator<<(std::ostream & os, const UblasVectorExpression<VE> & v)
   {
      unsigned int size = v().size();
      unsigned int w = os.width();
      std::ostringstream ss;
      ss.flags(os.flags());
      ss.imbue(os.getloc());
      ss.precision(os.precision());
      ss << "[" << std::setw(w) << std::left << v()(0);
      for (int i = 1; i < size; ++i) ss << " " << std::setw(w) << std::left << v()(i);
      ss << "]";
      return os << ss.str();
   }
   /// @}

   /// @name Time
   /// @{
   using Time = boost::posix_time::time_duration;
   using boost::posix_time::seconds;
   using boost::posix_time::minutes;
   using boost::posix_time::hours;

   using boost::posix_time::ptime;
   using boost::posix_time::time_from_string;
   using boost::posix_time::to_simple_string;

   using boost::posix_time::not_a_date_time;
   using boost::posix_time::neg_infin;
   using boost::posix_time::pos_infin;

   using boost::gregorian::date;

   using boost::local_time::time_zone_ptr;
   using boost::local_time::posix_time_zone;

   extern const ptime epoch;

   inline double dSeconds(const Time & d) 
   {
      return double(d.ticks())/Time::ticks_per_second();
   }

   inline ptime localTime(const Time & t, const time_zone_ptr localTz)
   {
      return boost::local_time::local_date_time(epoch + t, localTz).local_time();
   }

   inline ptime utcTime(Time t)
   {
      return (epoch + t);
   }

   ptime utcTime(ptime localTime, const time_zone_ptr localTz);

   inline Time timeFromLocalTime(ptime localTime, const time_zone_ptr localTz)
   {
      return (utcTime(localTime, localTz) - epoch);
   }
   /// @}

   /// @name LatLongs
   /// @{
   class LatLong
   {
      public:
         double lat_;
         double long_;
   };
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
