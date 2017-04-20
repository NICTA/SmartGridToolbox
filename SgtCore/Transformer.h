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

#ifndef TRANSFORMER_DOT_H
#define TRANSFORMER_DOT_H

#include <SgtCore/Branch.h>

namespace Sgt
{
    enum class TransformerType
    {
        DD,
        DGY,
        SINGLE_PHASE,
        VV,
        YY
    };

    class TransformerAbc : public BranchAbc
    {
        public:

            SGT_PROPS_INIT(TransformerAbc);
            SGT_PROPS_INHERIT(BranchAbc);

            static const std::string& sComponentType()
            {
                static std::string result("branch");
                return result;
            }

            TransformerAbc(
                    const Phases& phases0, const Phases& phases1,
                    const arma::Col<Complex>& nomTurnsRatio, const arma::Col<Complex>& offNomRatio,
                    const arma::Col<Complex>& ZL, const arma::Col<Complex>& YM) :
                BranchAbc(phases0, phases1),
                nomTurnsRatio_(nomTurnsRatio),
                offNomRatio_(offNomRatio),
                ZL_(ZL),
                YM_(YM)
            {
                // Empty.
            }

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            /// @name Overridden from BranchAbc:
            /// @{

            virtual arma::Mat<Complex> inServiceY() const override;

            /// @}

            /// @brief Transformer type.
            virtual TransformerType transformerType() const = 0;

            /// @brief Nominal turns ratio.
            const arma::Col<Complex>& nomTurnsRatio() const {return nomTurnsRatio_;}
            /// @brief Nominal turns ratio.
            void setNomTurnsRatio(const arma::Col<Complex>& nomTurnsRatio);
            /// @brief Nominal turns ratio.
            void setNomTurnsRatio(const Complex& nomTurnsRatio, arma::uword iWinding);

            SGT_PROP_GET_SET(nomTurnsRatio,
                    nomTurnsRatio, const arma::Col<Complex>&,
                    setNomTurnsRatio, const arma::Col<Complex>&);
            
            /// @brief Off nominal multiplier.
            const arma::Col<Complex>& offNomRatio() const {return offNomRatio_;}
            /// @brief Off nominal multiplier.
            void setOffNomRatio(const arma::Col<Complex>& offNomRatio);
            /// @brief Off nominal multiplier.
            void setOffNomRatio(const Complex& offNomRatio, arma::uword iWinding);

            SGT_PROP_GET_SET(offNomRatio,
                    offNomRatio, const arma::Col<Complex>&,
                    setOffNomRatio, const arma::Col<Complex>&);

            // @brief Turns ratio.
            arma::Col<Complex> turnsRatio() const {return nomTurnsRatio_ % offNomRatio_;} // % = elemtwise mult.

            SGT_PROP_GET(turnsRatio, turnsRatio, arma::Col<Complex>);

            /// @brief Leakage impedance.
            const arma::Col<Complex>& ZL() const {return ZL_;}
            /// @brief Leakage impedance.
            void setZL(const arma::Col<Complex>& ZL);
            /// @brief Leakage impedance.
            void setZL(const Complex& ZL, arma::uword iWinding);
            
            SGT_PROP_GET_SET(ZL,
                    ZL, const arma::Col<Complex>&,
                    setZL, const arma::Col<Complex>&);
            
            /// @brief Magnetising admittance.
            const arma::Col<Complex>& YM() const {return YM_;}
            /// @brief Magnetising admittance.
            void setYM(const arma::Col<Complex>& YM);
            /// @brief Magnetising admittance.
            void setYM(const Complex& YM, arma::uword iWinding);
            
            SGT_PROP_GET_SET(YM,
                    YM, const arma::Col<Complex>&,
                    setYM, const arma::Col<Complex>&);

            /// @brief Voltages across primary windings.
            virtual arma::Col<Complex> VWindings0() const = 0;

            SGT_PROP_GET(VWindings0, VWindings0, arma::Col<Complex>);

            /// @brief Voltages across secondary windings.
            virtual arma::Col<Complex> VWindings1() const = 0;

            SGT_PROP_GET(VWindings1, VWindings1, arma::Col<Complex>);

            /// @brief Load currents across primary windings.
            ///
            /// Excludes any circular currents that may exist.
            virtual arma::Col<Complex> IWindings0() const = 0;

            SGT_PROP_GET(IWindings0, VWindings0, arma::Col<Complex>);

            /// @brief Load currents across secondary windings.
            ///
            /// Excludes any circular currents that may exist.
            virtual arma::Col<Complex> IWindings1() const = 0;
            
            SGT_PROP_GET(IWindings1, VWindings1, arma::Col<Complex>);

        protected:

            void invalidate() const;
            
            void ensureValid() const;

        private:

            virtual arma::Mat<Complex> calcY() const = 0;

        private:

            arma::Col<Complex> nomTurnsRatio_;
            arma::Col<Complex> offNomRatio_;
            arma::Col<Complex> ZL_;
            arma::Col<Complex> YM_;

            mutable bool isValid_{false};
            mutable arma::Mat<Complex> Y_;
    };
}

#endif // TRANSFORMER_DOT_H
