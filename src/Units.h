#ifndef UNITS_DOT_H
#define UNITS_DOT_H

#include<ostream>
#include<sstream>

namespace Units
{
   template<typename T1, int m, int kg, int s, int A, int K>
   class UnitQuantity
   {
      public:
         T1 val;
   };

   template<typename T1, typename T2, int m, int kg, int s, int A, int K>
   auto operator*(const UnitQuantity<T1, m, kg, s, A, K> & q, const T2 & scalar) 
      -> Unit<decltype(q.val_ * scalar), m, kg, s, A, K>
   {
      return {q.val_ * scalar};
   };

   template<typename T1, typename T2, int m, int kg, int s, A, K>
   auto operator*(const T1 & scalar, const Unit<T2, m, kg, s, A, K> & q)
      -> Unit<decltype(q.val_ * scalar), m, kg, s, A, K>
   {
      return {q.val_ * scalar};
   };

   template<typename T1, typename T2, int m, int kg, int s, A, K>
   auto operator+(const Unit<T1, m, kg, s, A, K> & lhs, const Unit<T1, m, kg, s, A, K> & rhs)
      -> Unit<decltype(lhs.val_ * ths.val_), m, kg, s, A, K>
   {
      return {lhs.val_ + rhs.val_};
   };

   template<typename T1, typename T2, int m1, int m2, int kg1, int kg2, int s1, int s2, int A1, int A2, int K1, int K2>
   auto operator*(const Unit<T1, m1, kg1, s1, A1, K1> & lhs,
                  const Unit<T2, m2, kg2, s2, A2, K2> & rhs) 
      -> Unit<decltype(lhs.val_ * rhs.val_), m1 + m2, kg1 + kg2, s1 + s2, A1 + A2, K1 + K2>
   {
      return {lhs.val_ * rhs.val_};
   }

   template<typename T1, typename T2, int m1, int m2, int kg1, int kg2, int s1, int s2, int A1, int A2, int K1, int K2>
   auto operator/(const Unit<T1, m1, kg1, s1, A1, K1> & lhs,
                  const Unit<T2, m2, kg2, s2, A2, K2> & rhs) 
      -> Unit<decltype(lhs.val_ / rhs.val_), m1 - m2, kg1 - kg2, s1 - s2, A1 - A2, K1 - K2>
   {
      return {lhs.val_ / rhs.val_};
   }

   template<typename T, int m, int kg, int s, A, K>
   ostream & operator<<(ostream & os, const UnitQuantity<T, m, kg, s, A, K> & q)
   {
      std::ostringstream ss;
      ss << q.val << " ";
      if (m != 0) {ss << "m"; if (m != 1) ss << "^" << m;}
      if (kg != 0) {ss << "kg"; if (m != 1) ss << "^" << kg;}
      if (s != 0) {ss << "s"; if (m != 1) ss << "^" << s;}
      if (A != 0) {ss << "A"; if (A != 1) ss << "^" << A;}
      if (K != 0) {ss << "K"; if (K != 1) ss << "^" << K;}
      return os << ss.str();
   }

   template<typename T>
   ostream & operator<<(ostream & os, const UnitQuantity<T, 0, 0, 0, 0, 0> & q)
   {
      return os << q.val;
   }

   template<typename T> using Length = Unit<T, 1, 0, 0, 0, 0>;
   template<typename T> using Mass = Unit<T, 0, 1, 0, 0, 0>;
   template<typename T> using Time = Unit<T, 0, 0, 1, 0, 0>;
   template<typename T> using Current = Unit<T, 0, 0, 0, 1, 0>;
   template<typename T> using Temperature = Unit<T, 0, 0, 0, 0, 1>;

   Length<double> m = {1.0};
   Mass<double> kg = {1.0};
   Time<double> s = {1.0};
   Current<double> A = {1.0};
   Temerature<double> K = {1.0};
}

#endif // UNITS_DOT_H
