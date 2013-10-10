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
   inline std::ostream & messageStream() {return std::cout;}
   inline std::ostream & warningStream() {return std::cerr;}
   inline std::ostream & errorStream() {return std::cerr;}
   inline std::ostream & debugStream() {return std::cout;}

   inline std::ostream & message() {return messageStream()  <<  "MESSAGE:\t";}
   inline std::ostream & warning() {return warningStream()  <<  "WARNING:\t";}
   inline std::ostream & error() {return errorStream()      <<  "ERROR:  \t";}
   inline std::ostream & debug() {return debugStream()      <<  "DEBUG:  \t";}

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

   /// Dense vector
   template<typename T> using UblasVector = boost::numeric::ublas::vector<T>;

   /// Sparse compressed vector
   template<typename T> using UblasCVector = boost::numeric::ublas::compressed_vector<T>;

   /// Dense matrix 
   template<typename T> using UblasMatrix = boost::numeric::ublas::matrix<T>;

   /// Sparse compressed matrix, by rows.
   template<typename T> using UblasCMatrix = boost::numeric::ublas::compressed_matrix<T>;

   template<typename VE> using UblasVectorExpression = boost::numeric::ublas::vector_expression<VE>;
   template<typename ME> using UblasMatrixExpression = boost::numeric::ublas::matrix_expression<ME>;
   
   using UblasRange = boost::numeric::ublas::range;
   template<typename M> using UblasMatrixRange = boost::numeric::ublas::matrix_range<M>;

   using UblasSlice = boost::numeric::ublas::slice;
   template<typename M> using UblasMatrixSlice = boost::numeric::ublas::matrix_slice<M>;

   template<typename M> using UblasMatrixRow = boost::numeric::ublas::matrix_row<M>;
   template<typename M> using UblasMatrixColumn = boost::numeric::ublas::matrix_column<M>;

   using boost::numeric::ublas::project;
   using boost::numeric::ublas::row;
   using boost::numeric::ublas::column;

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
   using boost::posix_time::duration_from_string;

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

   inline Time timeFromUTCTime(ptime utcTime)
   {
      return (utcTime - epoch);
   }

   inline ptime utcTime(Time t)
   {
      return (epoch + t);
   }

   ptime utcTimeFromLocalTime(ptime localTime, const time_zone_ptr localTz);

   inline ptime localTime(const Time & t, const time_zone_ptr localTz)
   {
      return boost::local_time::local_date_time(epoch + t, localTz).local_time();
   }

   inline Time timeFromLocalTime(ptime localTime, const time_zone_ptr localTz)
   {
      return (utcTimeFromLocalTime(localTime, localTz) - epoch);
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
   extern const double pi;
   extern const double second;
   extern const double minute;
   extern const double hour;
   extern const double day;
   extern const double week;
   extern const double J;
   extern const double kJ;
   extern const double W;
   extern const double kW;
   extern const double kWh;
   extern const double A;
   extern const double C;
   extern const double K;
   extern const Complex czero;
   extern const LatLong Greenwich;
   /// @}

   /// @name Basic vector algebra in n dimensions.
   /// @{
   template<typename T, std::size_t d> double dot(const Array<T, d> & v1, const Array<T, d> & v2)
   {
      T result(0.0);
      for (std::size_t i = 0; i < d; ++i) result += v1[i] * v2[i];
      return result;
   }

   template<typename T, std::size_t d, typename S> Array<T, d> operator*(const Array<T, d> & v, const S & s)
   {
      Array<T, d> result = v;
      for (std::size_t i = 0; i < d; ++i) result[i] *= s;
      return result;
   }

   template<typename T, std::size_t d, typename S> Array<T, d> operator*(const S & s, const Array<T, d> & v)
   {
      return operator*(v, s);
   }

   template<typename T, std::size_t d> Array<T, d> operator+(const Array<T, d> & lhs, const Array<T, d> & rhs)
   {
      Array<T, d> result = lhs;
      for (std::size_t i = 0; i < d; ++i) result[i] += rhs[i];
      return result;
   }

   template<typename T, std::size_t d> Array<T, d> operator-(const Array<T, d> & lhs, const Array<T, d> & rhs)
   {
      Array<T, d> result = lhs;
      for (std::size_t i = 0; i < d; ++i) result[i] -= rhs[i];
      return result;
   }
   /// @}
}

#endif // COMMON_DOT_H
