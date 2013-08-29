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
      -> UnitQuantity<decltype(q.val * scalar), m, kg, s, A, K>
   {
      return {q.val * scalar};
   };

   template<typename T1, typename T2, int m, int kg, int s, int A, int K>
   auto operator*(const T1 & scalar, const UnitQuantity<T2, m, kg, s, A, K> & q)
      -> UnitQuantity<decltype(q.val * scalar), m, kg, s, A, K>
   {
      return {q.val * scalar};
   };

   template<typename T1, typename T2, int m, int kg, int s, int A, int K>
   auto operator+(const UnitQuantity<T1, m, kg, s, A, K> & lhs, const UnitQuantity<T1, m, kg, s, A, K> & rhs)
      -> UnitQuantity<decltype(lhs.val * rhs.val), m, kg, s, A, K>
   {
      return {lhs.val + rhs.val};
   };

   template<typename T1, typename T2, int m1, int m2, int kg1, int kg2, int s1, int s2, int A1, int A2, int K1, int K2>
   auto operator*(const UnitQuantity<T1, m1, kg1, s1, A1, K1> & lhs,
                  const UnitQuantity<T2, m2, kg2, s2, A2, K2> & rhs) 
      -> UnitQuantity<decltype(lhs.val * rhs.val), m1 + m2, kg1 + kg2, s1 + s2, A1 + A2, K1 + K2>
   {
      return {lhs.val * rhs.val};
   }

   template<typename T1, typename T2, int m1, int m2, int kg1, int kg2, int s1, int s2, int A1, int A2, int K1, int K2>
   auto operator/(const UnitQuantity<T1, m1, kg1, s1, A1, K1> & lhs,
                  const UnitQuantity<T2, m2, kg2, s2, A2, K2> & rhs) 
      -> UnitQuantity<decltype(lhs.val / rhs.val), m1 - m2, kg1 - kg2, s1 - s2, A1 - A2, K1 - K2>
   {
      return {lhs.val / rhs.val};
   }

   template<typename T, int m, int kg, int s, int A, int K>
   std::ostream & operator<<(std::ostream & os, const UnitQuantity<T, m, kg, s, A, K> & q)
   {
      const char dot = 187;
      std::ostringstream ss;
      ss << q.val << " ";
      bool prevUnit = false;
      if (m != 0) {ss << "m"; if (m != 1) ss << "^" << m; prevUnit = true;}
      if (kg != 0) {if (prevUnit) ss << u8"\u00B7"; ss << "kg"; if (kg != 1) ss << "^" << kg; prevUnit = true;}
      if (s != 0) {if (prevUnit) ss << u8"\u00B7";ss << "s"; if (s != 1) ss << "^" << s; prevUnit = true;}
      if (A != 0) {if (prevUnit) ss << u8"\u00B7";ss << "A"; if (A != 1) ss << "^" << A; prevUnit = true;}
      if (K != 0) {if (prevUnit) ss << u8"\u00B7";ss << "K"; if (K != 1) ss << "^" << K; prevUnit = true;}
      return os << ss.str();
   }

   template<typename T>
   std::ostream & operator<<(std::ostream & os, const UnitQuantity<T, 0, 0, 0, 0, 0> & q)
   {
      return os << q.val;
   }

   template<typename T> using Length = UnitQuantity<T, 1, 0, 0, 0, 0>;
   template<typename T> using Mass = UnitQuantity<T, 0, 1, 0, 0, 0>;
   template<typename T> using Time = UnitQuantity<T, 0, 0, 1, 0, 0>;
   template<typename T> using Current = UnitQuantity<T, 0, 0, 0, 1, 0>;
   template<typename T> using Temperature = UnitQuantity<T, 0, 0, 0, 0, 1>;

   Length<double> m = {1.0};
   Mass<double> kg = {1.0};
   Time<double> s = {1.0};
   Current<double> A = {1.0};
   Temperature<double> K = {1.0};
}

#endif // UNITS_DOT_H
