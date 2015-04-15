#ifndef COMMON_DOT_H
#define COMMON_DOT_H

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <armadillo>

#include <complex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#ifdef DEBUG
#define SGT_DEBUG(x) x
#else
#define SGT_DEBUG(x)
#endif

namespace Sgt
{
   /// @addtogroup Common
/// @{

/// @name Reporting and errors.
/// @{

   class StreamIndent : public std::streambuf
   {
      public:

         explicit StreamIndent(std::ostream& strm) :
            strm_(&strm),
            destBuf_(strm.rdbuf()),
            isFirst_(true),
            isNewline_(true),
            ind1_("    "),
            ind2_("    ")
         {
            strm_->rdbuf(this);
         }

         virtual ~StreamIndent()
         {
            if (strm_ != NULL)
            {
               strm_->rdbuf(destBuf_);
            }
         }

         void reset(const std::string& ind1, const std::string& ind2)
         {
            ind1_ = ind1;
            ind2_ = ind2;
            isFirst_ = true;
         }

      protected:

         virtual int overflow(int ch)
         {
            if (isNewline_ && ch != '\n')
            {
               if (isFirst_)
               {
                  destBuf_->sputn(ind1_.c_str(), ind1_.size());
               }
               else
               {
                  destBuf_->sputn(ind2_.c_str(), ind2_.size());
               }
            }
            isNewline_ = ch == '\n';
            isFirst_ = false;
            return destBuf_->sputc(ch);
         }

      private:

         std::ostream* strm_;
         std::streambuf* destBuf_;
         bool isFirst_;
         bool isNewline_;
         std::string ind1_;
         std::string ind2_;
   };

   class Log
   {
      friend class LogIndent;

      public:

         ~Log()
         {
            if (isFatal_)
            {
               cerrBuf_.reset("", "");
               std::cerr << std::endl << "ABORTING." << std::endl;
               abort();
            }
         }

         std::ostream& message()
         {
            coutBuf_.reset(
               std::string("MESSAGE: ") + std::string(indentLevel_, ' '),
               std::string("         ") + std::string(indentLevel_, ' '));
            return std::cout;
         }

         std::ostream& warning()
         {
            cerrBuf_.reset(
               std::string("WARNING: ") + std::string(indentLevel_, ' '),
               std::string("         ") + std::string(indentLevel_, ' '));
            return std::cerr;
         }

         std::ostream& debug()
         {
            cerrBuf_.reset(
               std::string("DEBUG  : ") + std::string(indentLevel_, ' '),
               std::string("         ") + std::string(indentLevel_, ' '));
            return std::cerr;
         }

         std::ostream& error()
         {
            cerrBuf_.reset(
               std::string("ERROR  : ") + std::string(indentLevel_, ' '),
               std::string("         ") + std::string(indentLevel_, ' '));
            return std::cerr;
         }

         std::ostream& fatal()
         {
            isFatal_ = true;
            cerrBuf_.reset(
               std::string("FATAL  : ") + std::string(indentLevel_, ' '),
               std::string("         ") + std::string(indentLevel_, ' '));
            return std::cerr;
         }

      private:

         static int indentLevel_;

         StreamIndent coutBuf_{std::cout};
         StreamIndent cerrBuf_{std::cerr};
         bool isFatal_{false};
   };

   class LogIndent
   {
      public:
         LogIndent()
         {
            Log::indentLevel_ += 4;
         }
         ~LogIndent()
         {
            Log::indentLevel_ -= 4;
         }
   };

/// @}

/// @name String conversion.
/// @{

   template<typename T> T from_string(const std::string& s);

/// @}

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

   inline Complex operator*(int i, const Complex& c)
   {
      return Complex(i * c.real(), i * c.imag());
   }

   inline Complex operator*(const Complex& c, int i)
   {
      return Complex(i * c.real(), i * c.imag());
   }

   std::ostream& operator<<(std::ostream& os, const Complex& c);

   template<> Complex from_string<Complex>(const std::string& s);

   std::string to_string(const Complex& c);

/// @}

/// @name Linear algebra
/// @{

   template<typename T> std::ostream& operator<<(std::ostream& os, const arma::Col<T>& v)
   {
      unsigned int size = v.size();
      unsigned int w = os.width();
      std::ostringstream ss;
      ss.flags(os.flags());
      ss.imbue(os.getloc());
      ss.precision(os.precision());
      ss << "[";
      if (v.size() > 0)
      {
         ss << std::setw(w) << std::left << v(0);
         for (int i = 1; i < size; ++i) ss << ", " << std::setw(w) << std::left << v(i);
      }
      ss << "]";
      return os << ss.str();
   }
   extern template std::ostream& operator<< <double>(std::ostream& os, const arma::Col<double>& v);
   extern template std::ostream& operator<< <float>(std::ostream& os, const arma::Col<float>& v);
   extern template std::ostream& operator<< <int>(std::ostream& os, const arma::Col<int>& v);
   extern template std::ostream& operator<< <Complex>(std::ostream& os, const arma::Col<Complex>& v);

   template<typename T> std::ostream& operator<<(std::ostream& os, const arma::Mat<T>& m)
   {
      unsigned int size1 = m.n_rows;
      unsigned int size2 = m.n_cols;
      unsigned int w = os.width();
      std::ostringstream ss;
      ss.flags(os.flags());
      ss.imbue(os.getloc());
      ss.precision(os.precision());
      ss << std::endl << "[" << std::endl;
                       ss << "    [" << std::setw(w) << std::left << m(0, 0);
                                      for (int j = 1; j < size2; ++j)
      {
         ss << ", " << std::setw(w) << std::left << m(0, j);
      }
                                  ss << "]";
                                  for (int i = 1; i < size1; ++i)
      {
         ss << "," << std::endl << "    [" << std::setw(w) << std::left << m(i, 0);
                                            for (int j = 1; j < size2; ++j)
         {
            ss << ", " << std::setw(w) << std::left << m(i, j);
         }
                                     ss << "]";
      }
                                  ss << std::endl << "]" << std::endl;
                                        return os << ss.str();
   }
                                     extern template std::ostream& operator<< <double>(std::ostream& os, const arma::Mat<double>& v);
                                     extern template std::ostream& operator<< <float>(std::ostream& os, const arma::Mat<float>& v);
                                     extern template std::ostream& operator<< <int>(std::ostream& os, const arma::Mat<int>& v);
                                     extern template std::ostream& operator<< <Complex>(std::ostream& os, const arma::Mat<Complex>& v);

                                  /// @}

                                  /// @name Time
                                  /// @{

                                     namespace posix_time = boost::posix_time;
                                     namespace gregorian = boost::gregorian;
                                     namespace local_time = boost::local_time;

                                     using Time = posix_time::time_duration;

                                     extern const posix_time::ptime epoch;

                                     inline double dSeconds(const Time& d)
   {
      return double(d.ticks()) / Time::ticks_per_second();
   }

   inline Time timeFromUtcTime(posix_time::ptime utcTime)
   {
      return (utcTime - epoch);
   }

   inline posix_time::ptime utcTime(Time t)
   {
      return (epoch + t);
   }

   posix_time::ptime utcTimeFromLocalTime(posix_time::ptime localTime, const local_time::time_zone_ptr localTz);

   inline posix_time::ptime localTime(const Time& t, const local_time::time_zone_ptr localTz)
   {
      return boost::local_time::local_date_time(epoch + t, localTz).local_time();
   }

   inline Time timeFromLocalTime(posix_time::ptime localTime, const local_time::time_zone_ptr localTz)
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
   extern const double negInfinity;
   extern const double infinity;
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

   template<typename T, std::size_t d> double dot(const Array<T, d>& v1, const Array<T, d>& v2)
   {
      T result(0.0);
      for (std::size_t i = 0; i < d; ++i) result += v1[i] * v2[i];
      return result;
   }

   template<typename T, std::size_t d, typename S> Array<T, d> operator*(const Array<T, d>& v, const S& s)
   {
      Array<T, d> result = v;
      for (std::size_t i = 0; i < d; ++i) result[i] *= s;
      return result;
   }

   template<typename T, std::size_t d, typename S> Array<T, d> operator*(const S& s, const Array<T, d>& v)
   {
      return operator*(v, s);
   }

   template<typename T, std::size_t d> Array<T, d> operator+(const Array<T, d>& lhs, const Array<T, d>& rhs)
   {
      Array<T, d> result = lhs;
      for (std::size_t i = 0; i < d; ++i) result[i] += rhs[i];
      return result;
   }

   template<typename T, std::size_t d> Array<T, d> operator-(const Array<T, d>& lhs, const Array<T, d>& rhs)
   {
      Array<T, d> result = lhs;
      for (std::size_t i = 0; i < d; ++i) result[i] -= rhs[i];
      return result;
   }

/// @}

/// @}
}

#endif // COMMON_DOT_H
