#ifndef UNITS_DOT_H
#define UNITS_DOT_H

#include<ostream>
#include<sstream>

/// Physical units.
/** Represents a nearly SI system of units. */
namespace Units
{
   constexpr const char * cdot = u8"\u00B7"; 
   template<typename T1, int m, int kg, int s, int A, int K>
   class DimensionalQuantity
   {
      public:
         typedef T1 ValueType;

      public:
         static constexpr int mPow() {return m;}
         static constexpr int kgPow() {return kg;}
         static constexpr int sPow() {return s;}
         static constexpr int APow() {return A;}
         static constexpr int KPow() {return K;}

      public:
         DimensionalQuantity(T1 stdVal) : stdVal_(stdVal) {}
         const T1 & stdVal() const {return stdVal_;}
         T1 & stdVal() {return stdVal_;}

      private:
         T1 stdVal_;
   };

   template<typename T1>
   class DimensionalQuantity<T1, 0, 0, 0, 0, 0>
   {
      public:
         typedef T1 ValueType;
      public:
         static constexpr int mPow() {return 0;}
         static constexpr int kgPow() {return 0;}
         static constexpr int sPow() {return 0;}
         static constexpr int APow() {return 0;}
         static constexpr int KPow() {return 0;}
         operator const T1 &() const {return val;} 
         operator T1 &() {return val;} 
      private:
         T1 val;
   };

   template<typename T1, typename T2, int m, int kg, int s, int A, int K>
   auto operator*(const DimensionalQuantity<T1, m, kg, s, A, K> & q, const T2 & scalar) 
      -> DimensionalQuantity<decltype(q.val * scalar), m, kg, s, A, K>
   {
      return {q.val * scalar};
   };

   template<typename T1, typename T2, int m, int kg, int s, int A, int K>
   auto operator*(const T1 & scalar, const DimensionalQuantity<T2, m, kg, s, A, K> & q)
      -> DimensionalQuantity<decltype(q.val * scalar), m, kg, s, A, K>
   {
      return {q.val * scalar};
   };

   template<typename T1, typename T2, int m, int kg, int s, int A, int K>
   auto operator+(const DimensionalQuantity<T1, m, kg, s, A, K> & lhs, const DimensionalQuantity<T1, m, kg, s, A, K> & rhs)
      -> DimensionalQuantity<decltype(lhs.val * rhs.val), m, kg, s, A, K>
   {
      return {lhs.val + rhs.val};
   };

   template<typename T1, typename T2, int m1, int m2, int kg1, int kg2, int s1, int s2, int A1, int A2, int K1, int K2>
   auto operator*(const DimensionalQuantity<T1, m1, kg1, s1, A1, K1> & lhs,
                  const DimensionalQuantity<T2, m2, kg2, s2, A2, K2> & rhs) 
      -> DimensionalQuantity<decltype(lhs.val * rhs.val), m1 + m2, kg1 + kg2, s1 + s2, A1 + A2, K1 + K2>
   {
      return {lhs.val * rhs.val};
   }

   template<typename T1, typename T2, int m1, int m2, int kg1, int kg2, int s1, int s2, int A1, int A2, int K1, int K2>
   auto operator/(const DimensionalQuantity<T1, m1, kg1, s1, A1, K1> & lhs,
                  const DimensionalQuantity<T2, m2, kg2, s2, A2, K2> & rhs) 
      -> DimensionalQuantity<decltype(lhs.val / rhs.val), m1 - m2, kg1 - kg2, s1 - s2, A1 - A2, K1 - K2>
   {
      return {lhs.val / rhs.val};
   }

   template<typename T1, int m, int kg, int s, int A, int K>
   std::ostream & operator<<(std::ostream & os, const DimensionalQuantity<T1, m, kg, s, A, K> & q)
   {
      const char dot = 187;
      std::ostringstream ss;
      ss << q.val << " ";
      bool prevUnit = false;
      if (m != 0) {ss << "m"; if (m != 1) ss << "^" << m; prevUnit = true;}
      if (kg != 0) {if (prevUnit) ss << cdot << ss << "kg"; if (kg != 1) ss << "^" << kg; prevUnit = true;}
      if (s != 0) {if (prevUnit) ss << cdot << "s"; if (s != 1) ss << "^" << s; prevUnit = true;}
      if (A != 0) {if (prevUnit) ss << cdot << "A"; if (A != 1) ss << "^" << A; prevUnit = true;}
      if (K != 0) {if (prevUnit) ss << cdot << "K"; if (K != 1) ss << "^" << K; prevUnit = true;}
      return os << ss.str();
   }

   template<typename T1> using Length = DimensionalQuantity<T1, 1, 0, 0, 0, 0>;
   template<typename T1> using Mass = DimensionalQuantity<T1, 0, 1, 0, 0, 0>;
   template<typename T1> using Time = DimensionalQuantity<T1, 0, 0, 1, 0, 0>;
   template<typename T1> using Current = DimensionalQuantity<T1, 0, 0, 0, 1, 0>;
   template<typename T1> using Temperature = DimensionalQuantity<T1, 0, 0, 0, 0, 1>;

   template<typename T1, int m, int kg, int s, int A, int K>
   class Unit : public DimensionalQuantity<T1, m, kg, s, A, K>
   {
      public:
         Unit(T1 & stdVal, const std::string & name) :
            DimensionalQuantity<T1, m, kg, s, A, K>(stdVal), name_(name) 
         {}
         Unit(const DimensionalQuantity<T1, m, kg, s, A, K> & q, const std::string & name) :
            DimensionalQuantity<T1, m, kg, s, A, K>(q),
            name_(name) 
         {}
         const std::string & name() {return name_;}

      private:
         std::string name_;
   };

   template<typename T1, typename T2, int m, int kg, int s, int A, int K>
   class UnitQuantity
   {
      public:
         UnitQuantity(const DimensionalQuantity<T1, m, kg, s, A, K> & q, const Unit<T2, m, kg, s, A, K> & u) :
            q_(q), u_(u)
         {}
         friend std::ostream & operator<<(std::ostream & os, const UnitQuantity & uq)
         {
            return os << (uq.q_ / uq.u_) << " " << uq.u_.name();
         }
      private:
         const DimensionalQuantity<T1, m, kg, s, A, K> & q_;
         const Unit<T2, m, kg, s, A, K> & u_;
   };

   template<typename T1> using Length = DimensionalQuantity<T1, 1, 0, 0, 0, 0>;
   template<typename T1> using Mass = DimensionalQuantity<T1, 0, 1, 0, 0, 0>;
   template<typename T1> using Time = DimensionalQuantity<T1, 0, 0, 1, 0, 0>;
   template<typename T1> using Current = DimensionalQuantity<T1, 0, 0, 0, 1, 0>;
   template<typename T1> using Temperature = DimensionalQuantity<T1, 0, 0, 0, 0, 1>;

   extern const Unit<double, 1, 0, 0, 0, 0> m = {1.0, "m"};
   extern const Unit<double, 0, 1, 0, 0, 0> kg = {1.0, "kg"};
   extern const Unit<double, 0, 0, 1, 0, 0> s = {1.0, "s"};
   extern const Unit<double, 0, 0, 0, 1, 0> A = {1.0, "A"};
   extern const Unit<double, 0, 0, 0, 0, 1> K = {1.0, "K"};
   extern const Unit<double, 1, 0, -1, 0, 0> m_per_sec = {m.stdVal() / s.stdVal(), std::string("m") + cdot + "s^-1"};
}

#endif // UNITS_DOT_H
