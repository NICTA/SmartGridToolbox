// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef UNITS_DOT_H
#define UNITS_DOT_H

#include<ostream>
#include<sstream>

/// @brief Physical units.
///
/// Represents a nearly SI system of units.
namespace Units
{
    constexpr const char* cdot = u8"\u00B7";

    /// @ingroup Utilities
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
            static constexpr auto pow() -> Dimensions<L* p, M* p, T* p, I* p, Th* p>
            {
                return Dimensions<L* p, M* p, T* p, I* p, Th* p>();
            }
    };

    template<int LL, int ML, int TL, int IL, int ThL, int LR, int MR, int TR, int IR, int ThR>
    static constexpr auto operator*(const Dimensions<LL, ML, TL, IL, ThL>& lhs,
                                    const Dimensions<LR, MR, TR, IR, ThR>& rhs)
    -> Dimensions<LL + LR, ML + MR, TL + TR, IL + IR, ThL + ThR>
    {
        return Dimensions<LL + LR, ML + MR, TL + TR, IL + IR, ThL + ThR>();
    }

    template<int LL, int ML, int TL, int IL, int ThL, int LR, int MR, int TR, int IR, int ThR>
    static constexpr auto operator/(const Dimensions<LL, ML, TL, IL, ThL>& lhs,
                                    const Dimensions<LR, MR, TR, IR, ThR>& rhs)
    -> Dimensions<LL - LR, ML - MR, TL - TR, IL - IR, ThL - ThR>
    {
        return Dimensions<LL - LR, ML - MR, TL - TR, IL - IR, ThL - ThR>();
    }

    // Base dimensions.
    constexpr Dimensions<1, 0, 0, 0, 0> LDim() {return decltype(LDim())();}
    constexpr Dimensions<0, 1, 0, 0, 0> MDim() {return decltype(MDim())();}
    constexpr Dimensions<0, 0, 1, 0, 0> TDim() {return decltype(TDim())();}
    constexpr Dimensions<0, 0, 0, 1, 0> IDim() {return decltype(IDim())();}
    constexpr Dimensions<0, 0, 0, 0, 1> ThDim() {return decltype(ThDim())();}

    // Derived dimensions.
    constexpr decltype(LDim() / TDim()) vDim() {return decltype(vDim())();}
    constexpr decltype(LDim() / TDim().pow<2>()) aDim() {return decltype(aDim())();}
    constexpr decltype(MDim() * vDim()) pDim() {return decltype(pDim())();}
    constexpr decltype(MDim() * aDim()) fDim() {return decltype(fDim())();}
    constexpr decltype(fDim() * LDim()) EDim() {return decltype(EDim())();}
    constexpr decltype(EDim() / TDim()) PDim() {return decltype(PDim())();}
    constexpr decltype(IDim() * TDim()) QDim() {return decltype(QDim())();}
    constexpr decltype(QDim() / LDim()) VDim() {return decltype(VDim())();}

    /// @ingroup Utilities
    template<typename D, typename V = double>
    class DimensionalQuantity : public D
    {
        public:
            typedef V ValType;

        public:
            DimensionalQuantity(const D& dim, const V& stdVal) : stdVal_(stdVal) {}

            const V& stdVal() const {return stdVal_;}
            V& stdVal() {return stdVal_;}

        private:
            V stdVal_;
    };

    /// @ingroup Utilities
    template<typename V>
    class DimensionalQuantity<Dimensions<0, 0, 0, 0, 0>, V>
    {
        public:
            typedef V ValType;

        public:
            DimensionalQuantity(V stdVal) : stdVal_(stdVal) {}

            const V& stdVal() const {return stdVal_;}
            V& stdVal() {return stdVal_;}

            operator const V &() const {return stdVal_;}
            operator V &() {return stdVal_;}

        private:
            V stdVal_;
    };

    template<typename D, typename V, typename V2>
    auto operator*(const DimensionalQuantity<D, V>& q, const V2& val)
    -> DimensionalQuantity<D, decltype(q.stdVal() * val)>
    {
        return {q.stdVal() * val};
    };

    template<typename D, typename V, typename V2>
    auto operator*(const V2& scalar, const DimensionalQuantity<D, V>& q)
    -> DimensionalQuantity<D, decltype(q.stdVal() * scalar)>
    {
        return {q.stdVal() * scalar};
    };

    template<typename D, typename V1, typename V2>
    auto operator+(const DimensionalQuantity<D, V1>& lhs, const DimensionalQuantity<D, V2>& rhs)
    -> DimensionalQuantity<D, decltype(lhs.stdVal() * rhs.stdVal())>
    {
        return {lhs.stdVal() + rhs.stdVal()};
    };

    template<typename D1, typename D2, typename V1, typename V2>
    auto operator*(const DimensionalQuantity<D1, V1>& lhs, const DimensionalQuantity<D2, V2>& rhs)
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
    auto operator/(const DimensionalQuantity<D1, V1>& lhs, const DimensionalQuantity<D2, V2>& rhs)
    -> DimensionalQuantity<Dimensions<D1::LDim() - D2::LDim(),
    D1::MDim() - D2::MDim(),
    D1::TDim() - D2::TDim(),
    D1::IDim() - D2::IDim(),
    D1::ThDim() - D2::ThDim()>,
    decltype(lhs.stdVal() / rhs.stdVal())>
    {
        return {lhs.stdVal() / rhs.stdVal()};
    }

    template<typename V = double> using Length = DimensionalQuantity<decltype(LDim()), V>;
    template<typename V = double> using Mass = DimensionalQuantity<decltype(MDim()), V>;
    template<typename V = double> using Time = DimensionalQuantity<decltype(TDim()), V>;
    template<typename V = double> using Current = DimensionalQuantity<decltype(IDim()), V>;
    template<typename V = double> using Temperature = DimensionalQuantity<decltype(ThDim()), V>;

    /// @ingroup Utilities
    template<typename D, typename V = double>
    class Unit : public DimensionalQuantity<D, V>
    {
        public:
            Unit(const DimensionalQuantity<D, V>& q, const std::string& name) :
                DimensionalQuantity<D, V>(q),
                name_(name)
            {
                // Empty.
            }

            const std::string& name() const {return name_;}

        private:
            std::string name_;
    };

    namespace SI
    {
        // Base SI units.
        extern const Unit<decltype(LDim())> m;
        extern const Unit<decltype(MDim())> kg;
        extern const Unit<decltype(TDim())> s;
        extern const Unit<decltype(IDim())> A;
        extern const Unit<decltype(ThDim())> K;

        extern const Unit<decltype(QDim())> C;
    }

    /// @ingroup Utilities
    template<typename D, typename V = double, typename V2 = double>
    class UnitQuantity
    {
        public:
            UnitQuantity(const DimensionalQuantity<D, V>& q, const Unit<D, V2>& u) : q_(q), u_(u) {}

            friend std::ostream& operator<<(std::ostream& os, const UnitQuantity& uq)
            {
                return os << (uq.q_ / uq.u_) << " " << uq.u_.name();
            }
        private:
            const DimensionalQuantity<D, V>& q_;
            const Unit<D, V2>& u_;
    };
}

#endif // UNITS_DOT_H
