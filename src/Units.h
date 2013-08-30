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
         typedef Dimensions D;

      public:
         static constexpr int LDim() {return L;}
         static constexpr int MDim() {return M;}
         static constexpr int TDim() {return T;}
         static constexpr int IDim() {return I;}
         static constexpr int ThDim() {return Th;}

         template<int p>
         static constexpr auto pow() -> Dimensions<L * p, M * p, T * p, I * p, Th * p>
         {
            return Dimensions<L * p, M * p, T * p, I * p, Th * p>();
         }
   };

   template<int LL, int ML, int TL, int IL, int ThL, int LR, int MR, int TR, int IR, int ThR>
   static constexpr auto operator*(const Dimensions<LL, ML, TL, IL, ThL> & lhs,
                                   const Dimensions<LR, MR, TR, IR, ThR> & rhs)
      -> Dimensions<LL + LR, ML + MR, TL + TR, IL + IR, ThL + ThR>
   {
      return Dimensions<LL + LR, ML + MR, TL + TR, IL + IR, ThL + ThR>();
   }

   template<int LL, int ML, int TL, int IL, int ThL, int LR, int MR, int TR, int IR, int ThR>
   static constexpr auto operator/(const Dimensions<LL, ML, TL, IL, ThL> & lhs,
                                   const Dimensions<LR, MR, TR, IR, ThR> & rhs)
      -> Dimensions<LL - LR, ML - MR, TL - TR, IL - IR, ThL - ThR>
   {
      return Dimensions<LL - LR, ML - MR, TL - TR, IL - IR, ThL - ThR>();
   }

   using LDimType = Dimensions<1, 0, 0, 0, 0>;
   using MDimType = Dimensions<0, 1, 0, 0, 0>;
   using TDimType = Dimensions<0, 0, 1, 0, 0>;
   using IDimType = Dimensions<0, 0, 0, 1, 0>;
   using ThDimType = Dimensions<0, 0, 0, 0, 0>;

   constexpr LDimType LDim() {return LDimType();}
   constexpr MDimType MDim() {return MDimType();}
   constexpr TDimType TDim() {return TDimType();}
   constexpr IDimType IDim() {return IDimType();}
   constexpr ThDimType ThDim() {return ThDimType();}

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

   template<typename V = double> using Length = DimensionalQuantity<LDimType, V>;
   template<typename V = double> using Mass = DimensionalQuantity<MDimType, V>;
   template<typename V = double> using Time = DimensionalQuantity<TDimType, V>;
   template<typename V = double> using Current = DimensionalQuantity<IDimType, V>;
   template<typename V = double> using Temperature = DimensionalQuantity<ThDimType, V>;

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

   extern const Unit<LDimType> m = {1.0, "m"};
   extern const Unit<MDimType> kg = {1.0, "kg"};
   extern const Unit<TDimType> s = {1.0, "s"};
   extern const Unit<IDimType> A = {1.0, "A"};
   extern const Unit<ThDimType> K = {1.0, "K"};

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
