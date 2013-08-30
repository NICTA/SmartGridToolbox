#ifndef UNITS_DOT_H
#define UNITS_DOT_H

#include<ostream>
#include<sstream>

/// Physical units.
/** Represents a nearly SI system of units. */
namespace Units
{
   constexpr const char * cdot = u8"\u00B7"; 

   template<int L, int M, int T, int I, int Th>
   class Dimensions
   {
      public:
         typedef Dimensions DimType;

      public:
         static constexpr int LDim() {return L;}
         static constexpr int MDim() {return M;}
         static constexpr int TDim() {return T;}
         static constexpr int IDim() {return I;}
         static constexpr int ThDim() {return Th;}
   };

   using LDim = Dimensions<1, 0, 0, 0, 0>;
   using MDim = Dimensions<0, 1, 0, 0, 0>;
   using TDim = Dimensions<0, 0, 1, 0, 0>;
   using IDim = Dimensions<0, 0, 0, 1, 0>;
   using ThDim = Dimensions<0, 0, 0, 0, 0>;

   template<typename D, typename V = double>
   class DimensionalQuantity : public D 
   {
      public:
         typedef V ValType;

      public:
         DimensionalQuantity(V stdVal) : stdVal_(stdVal) {}

         const V & stdVal() const {return stdVal_;}
         V & stdVal() {return stdVal_;}

      private:
         V stdVal_;
   };

   template<typename V>
   class DimensionalQuantity<Dimensions<0, 0, 0, 0, 0>, V>
   {
      public:
         typedef V ValType;

      public:
         DimensionalQuantity(V stdVal) : stdVal_(stdVal) {}

         const V & stdVal() const {return stdVal_;}
         V & stdVal() {return stdVal_;}

         operator const V &() const {return stdVal_;} 
         operator V &() {return stdVal_;} 

      private:
         V stdVal_;
   };

   template<typename D, typename V, typename V2>
   auto operator*(const DimensionalQuantity<D, V> & q, const V2 & val) 
      -> DimensionalQuantity<D, decltype(q.stdVal() * val)>
   {
      return {q.stdVal() * val};
   };

   template<typename D, typename V, typename V2>
   auto operator*(const V2 & scalar, const DimensionalQuantity<D, V> & q)
      -> DimensionalQuantity<D, decltype(q.stdVal() * scalar)>
   {
      return {q.stdVal() * scalar};
   };

   template<typename D, typename V1, typename V2>
   auto operator+(const DimensionalQuantity<D, V1> & lhs, const DimensionalQuantity<D, V2> & rhs)
      -> DimensionalQuantity<D, decltype(lhs.stdVal() * rhs.stdVal())>
   {
      return {lhs.stdVal() + rhs.stdVal()};
   };

   template<typename D1, typename D2, typename V1, typename V2>
   auto operator*(const DimensionalQuantity<D1, V1> & lhs, const DimensionalQuantity<D2, V2> & rhs) 
      -> DimensionalQuantity<Dimensions<D1::LDim() + D2::LDim(),
                                        D1::MDim() + D2::MDim(),
                                        D1::TDim() + D2::TDim(),
                                        D1::IDim() + D2::IDim(),
                                        D1::ThDim() + D2::ThDim()>,
                             decltype(lhs.stdVal() * rhs.stdVal())>
   {
      return {lhs.stdVal() * rhs.stdVal()};
   }

   template<typename V1, typename D1, typename V2, typename D2>
   auto operator/(const DimensionalQuantity<D1, V1> & lhs, const DimensionalQuantity<D2, V2> & rhs) 
      -> DimensionalQuantity<Dimensions<D1::LDim() - D2::LDim(),
                                        D1::MDim() - D2::MDim(),
                                        D1::TDim() - D2::TDim(),
                                        D1::IDim() - D2::IDim(),
                                        D1::ThDim() - D2::ThDim()>,
                             decltype(lhs.stdVal() / rhs.stdVal())>
   {
      return {lhs.stdVal() / rhs.stdVal()};
   }

   template<typename V = double> using Length = DimensionalQuantity<LDim, V>;
   template<typename V = double> using Mass = DimensionalQuantity<MDim, V>;
   template<typename V = double> using Time = DimensionalQuantity<TDim, V>;
   template<typename V = double> using Current = DimensionalQuantity<IDim, V>;
   template<typename V = double> using Temperature = DimensionalQuantity<ThDim, V>;

   template<typename D, typename V = double>
   class Unit : public DimensionalQuantity<D, V>
   {
      public:
         Unit(V & stdVal, const std::string & name) : DimensionalQuantity<D, V>(stdVal), name_(name) {}
         Unit(const DimensionalQuantity<D, V> & q, const std::string & name) :
            DimensionalQuantity<D, V>(q), name_(name) {}

         const std::string & name() const {return name_;}

      private:
         std::string name_;
   };

   extern const Unit<LDim> m = {1.0, "m"};
   extern const Unit<MDim> kg = {1.0, "kg"};
   extern const Unit<TDim> s = {1.0, "s"};
   extern const Unit<IDim> A = {1.0, "A"};
   extern const Unit<ThDim> K = {1.0, "K"};

   template<typename D, typename V = double, typename V2 = double>
   class UnitQuantity
   {
      public:
         UnitQuantity(const DimensionalQuantity<D, V> & q, const Unit<D, V2> & u) : q_(q), u_(u) {}

         friend std::ostream & operator<<(std::ostream & os, const UnitQuantity & uq)
         {
            return os << (uq.q_ / uq.u_) << " " << uq.u_.name();
         }
      private:
         const DimensionalQuantity<D, V> & q_;
         const Unit<D, V2> & u_;
   };
}

#endif // UNITS_DOT_H
