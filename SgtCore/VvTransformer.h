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

#ifndef VV_TRANSFORMER_DOT_H
#define VV_TRANSFORMER_DOT_H

#include <SgtCore/Branch.h>

namespace Sgt
{
    /// @brief V-V / open Delta - open Delta transformer.
    ///
    /// Windings are between terminals 2-1 and 2-3, so that terminal 2 forms the point of the "V".
    /// @ingroup PowerFlowCore
    class VvTransformer : public BranchAbc
    {
        public:

            SGT_PROPS_INIT(VvTransformer);
            SGT_PROPS_INHERIT(BranchAbc);

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("vv_transformer");
                return result;
            }

        /// @}

        /// @name Lifecycle
        /// @{

            /// @brief Constructor
            /// @param nomRatio Nominal complex voltage/turns ratio for windings.
            /// @param offNomRatio21 Off-nominal complex voltage/turns ratio for windings between phases 2 and 1.
            /// @param offNomRatio23 Off-nominal complex voltage/turns ratio for windings between phases 2 and 3.
            /// @param ZL The leakage impedance, must be > 0.
            /// @param phases0 The phases on the first (primary) side.
            /// @param phases1 The phases on the second (secondary) side.
            VvTransformer(const std::string& id, Complex nomRatio,
                    Complex offNomRatio21, Complex offNomRatio23, Complex ZL, Complex YTie, Complex YGround,
                    const Phases& phases0 = {Phase::A, Phase::B, Phase::C},
                    const Phases& phases1 = {Phase::A, Phase::B, Phase::C}) :
                Component(id),
                BranchAbc(phases0, phases1),
                nomRatio_(nomRatio),
                offNomRatio21_(offNomRatio21),
                offNomRatio23_(offNomRatio23),
                YL_(1.0/ZL),
                YTie_(YTie),
                YGround_(YGround)
            {
                // Empty.
            }

        /// @}

        /// @name Component virtual overridden member functions.
        /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            // virtual json toJson() const override // TODO

        /// @}

        /// @name Parameters:
        /// @{

            Complex nomRatio() const
            {
                return nomRatio_;
            }

            void setNomRatio(Complex nomRatio);
            
            SGT_PROP_GET_SET(nomRatio, nomRatio, Complex, setNomRatio, Complex);

            Complex offNomRatio21() const
            {
                return offNomRatio21_;
            }
            
            void setOffNomRatio21(Complex offNomRatio21);
            
            SGT_PROP_GET_SET(offNomRatio21, offNomRatio21, Complex, setOffNomRatio21, Complex);

            Complex offNomRatio23() const
            {
                return offNomRatio23_;
            }

            void setOffNomRatio23(Complex offNomRatio23);
            
            SGT_PROP_GET_SET(offNomRatio23, offNomRatio23, Complex, setOffNomRatio23, Complex);

            Complex a21() const
            {
                return offNomRatio21_ * nomRatio_;
            }
            
            SGT_PROP_GET(a21, a21, Complex);

            Complex a23() const
            {
                return offNomRatio23_ * nomRatio_;
            }
            
            SGT_PROP_GET(a23, a23, Complex);

            Complex ZL() const
            {
                return 1.0 / YL_;
            }
            
            void setZL(Complex ZL);
            
            SGT_PROP_GET_SET(ZL, ZL, Complex, setZL, Complex);

            Complex YTie() const
            {
                return YTie_;
            }
            
            void setYTie(Complex YTie);
            
            SGT_PROP_GET_SET(YTie, YTie, Complex, setYTie, Complex);

            Complex YGround() const
            {
                return YGround_;
            }
            
            void setYGround(Complex YGround);
            
            SGT_PROP_GET_SET(YGround, YGround, Complex, setYGround, Complex);

        /// @}

        /// @name Overridden from BranchAbc:
        /// @{

            virtual arma::Mat<Complex> inServiceY() const override;

        /// @}

        /// @name Private member functions
        /// @{

        private:

            void validate() const;

        /// @}

        private:

            Complex nomRatio_; ///< Nominal voltage ratio, V_nom,p/V_nom,s
            Complex offNomRatio21_; ///< Off nominal mulitplier for nomRatio_, winding between phases 2 and 1.
            Complex offNomRatio23_; ///< Off nominal mulitplier for nomRatio_, winding between phases 2 and 3.
            Complex YL_; ///< Series leakage admittance.
            Complex YTie_; ///< Admittance tying terminals 2 between primary and secondary.
            Complex YGround_; ///< Admittance tying secondary terminal 2 to ground.

            mutable bool isValid_{false};
            mutable arma::Mat<Complex> Y_;
    };
}

#endif // VV_TRANSFORMER_DOT_H
