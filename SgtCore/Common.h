#ifndef COMMON_DOT_H
#define COMMON_DOT_H

#include <complex>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>

#ifdef DEBUG
#define LPF_DEBUG(x) x
#else
#define LPF_DEBUG(x)
#endif

namespace LibPowerFlow
{
   /// @addtogroup Common
   /// @{

   /// @name Reporting and errors.
   /// @{

   class IndentingOStreamBuf : public std::streambuf
   {
      public:

         explicit IndentingOStreamBuf(std::ostream& dest, const std::string& ind) :
            dest_(dest.rdbuf()),
            isAtStartOfLine_(true),
            ind_(ind),
            owner_(&dest)
         {
            owner_->rdbuf(this);
         }

         virtual ~IndentingOStreamBuf()
         {
            if (owner_ != NULL) {
               owner_->rdbuf(dest_);
            }
         }

         void setInd(const std::string& ind) {
            ind_ = ind;
         }

      protected:

         virtual int overflow(int ch)
         {
            if (isAtStartOfLine_ && ch != '\n') {
               dest_->sputn(ind_.data(), ind_.size());
            }
            isAtStartOfLine_ = ch == '\n';
            return dest_->sputc(ch);
         }

      private:

         std::streambuf* dest_;
         bool isAtStartOfLine_;
         std::string ind_;
         std::ostream* owner_;
   };

   inline std::ostream& messageStream() {return std::cout;}
   inline std::ostream& warningStream() {return std::cerr;}
   inline std::ostream& errorStream() {return std::cerr;}
   inline std::ostream& debugStream() {return std::cout;}

   inline std::ostream& message() {return messageStream()  <<  "MESSAGE:\t";}
   inline std::ostream& warning() {return warningStream()  <<  "WARNING:\t";}
   inline std::ostream& error() {return errorStream()      <<  "ERROR:  \t";}
   inline std::ostream& debug() {return debugStream()      <<  "DEBUG:  \t";}

   inline void abort() {std::cerr << "ABORTING." << std::endl; ::abort();}
   
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
      return Complex(m*cos(theta), m*sin(theta));
   }

   inline Complex operator*(int i, const Complex& c)
   {
      return Complex(i*c.real(), i*c.imag());
   }

   inline Complex operator*(const Complex& c, int i)
   {
      return Complex(i*c.real(), i*c.imag());
   }

   std::ostream& operator<<(std::ostream& os, const Complex& c);

   Complex string2Complex(const std::string& s);

   std::string complex2String(const Complex& c);
   
   /// @}

   /// @name Linear algebra
   /// @{
   
   namespace ublas = boost::numeric::ublas;

   template<class T>
   bool invertMatrix(const ublas::matrix<T>& input, ublas::matrix<T>& inverse)
   {
      ublas::matrix<T> A(input);
      ublas::permutation_matrix<std::size_t> pm(A.size1());
      int res = ublas::lu_factorize(A, pm);
      if (res != 0)
      {
         return false;
      }
      inverse.assign(ublas::identity_matrix<T>(A.size1()));
      ublas::lu_substitute(A, pm, inverse);
      return true;
   }

   template<typename VE> std::ostream& operator<<(std::ostream& os, const ublas::vector_expression<VE>& v)
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

   /// @name Constants
   /// @{
   
   extern const double pi;
   extern const double negInfinity;
   extern const double infinity;
   extern const Complex czero;
   
   /// @}

   /// @name Basic vector algebra in n dimensions.
   /// @{
   
   template<typename T, std::size_t d> double dot(const Array<T, d>& v1, const Array<T, d>& v2)
   {
      T result(0.0);
      for (std::size_t i = 0; i < d; ++i) result += v1[i]*v2[i];
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
