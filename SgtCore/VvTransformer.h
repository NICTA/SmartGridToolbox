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

#include <SgtCore/Transformer.h>

namespace Sgt
{
    /// @brief V-V / open Delta - open Delta transformer.
    ///
    /// Windings are between terminals 2-1 and 2-3, so that terminal 2 forms the point of the "V".
    /// @ingroup PowerFlowCore
    class VvTransformer : public TransformerAbc
    {
        public:

            SGT_PROPS_INIT(VvTransformer);
            SGT_PROPS_INHERIT(TransformerAbc);

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
            /// @param offNomRatio10 Off-nominal complex voltage/turns ratio for windings between phases 1 and 0.
            /// @param offNomRatio12 Off-nominal complex voltage/turns ratio for windings between phases 1 and 2.
            /// @param ZL10 The leakage impedance between phases 1 and 0, must be > 0.
            /// @param ZL12 The leakage impedance between phases 1 and 2, must be > 0.
            /// @param YTie Tie admittance for center phase.
            /// @param YGround Ground admittance for center phase.
            /// @param phases0 The phases on the first (primary) side.
            /// @param phases1 The phases on the second (secondary) side.
            VvTransformer(const std::string& id, Complex nomRatio,
                    Complex offNomRatio10, Complex offNomRatio12,
                    Complex ZL10, Complex ZL12,
                    Complex YTie, Complex YGround,
                    const Phases& phases0 = {Phase::A, Phase::B, Phase::C},
                    const Phases& phases1 = {Phase::A, Phase::B, Phase::C}) :
                Component(id),
                TransformerAbc(phases0, phases1),
                nomRatio_(nomRatio),
                offNomRatio10_(offNomRatio10),
                offNomRatio12_(offNomRatio12),
                YL10_(1.0/ZL10),
                YL12_(1.0/ZL12),
                YTie_(YTie),
                YGround_(YGround)
            {
                // Empty.
            }

        /// @}
        
        /// @name Overridden from TransformerAbc:
        /// @{
            
            virtual TransformerType transformerType() const override {return TransformerType::VV;}
            virtual arma::Col<Complex> VWindings0() const override;
            virtual arma::Col<Complex> VWindings1() const override;
            virtual arma::Col<Complex> IWindings0() const override;
            virtual arma::Col<Complex> IWindings1() const override;

        /// @}

        /// @name Component virtual overridden member functions.
        /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            // virtual json toJson() const override; // TODO

        /// @}

        /// @name Parameters:
        /// @{

            Complex nomRatio() const
            {
                return nomRatio_;
            }

            void setNomRatio(Complex nomRatio);
            
            SGT_PROP_GET_SET(nomRatio, nomRatio, Complex, setNomRatio, Complex);

            Complex offNomRatio10() const
            {
                return offNomRatio10_;
            }
            
            void setOffNomRatio10(Complex offNomRatio10);
            
            SGT_PROP_GET_SET(offNomRatio10, offNomRatio10, Complex, setOffNomRatio10, Complex);

            Complex offNomRatio12() const
            {
                return offNomRatio12_;
            }

            void setOffNomRatio12(Complex offNomRatio12);
            
            SGT_PROP_GET_SET(offNomRatio12, offNomRatio12, Complex, setOffNomRatio12, Complex);

            Complex a10() const
            {
                return offNomRatio10_ * nomRatio_;
            }
            
            SGT_PROP_GET(a10, a10, Complex);

            Complex a12() const
            {
                return offNomRatio12_ * nomRatio_;
            }
            
            SGT_PROP_GET(a12, a12, Complex);

            Complex ZL10() const
            {
                return 1.0 / YL10_;
            }
            
            void setZL10(Complex ZL10);
            
            SGT_PROP_GET_SET(ZL10, ZL10, Complex, setZL10, Complex);

            Complex ZL12() const
            {
                return 1.0 / YL12_;
            }
            
            void setZL12(Complex ZL12);
            
            SGT_PROP_GET_SET(ZL12, ZL12, Complex, setZL12, Complex);

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

        /// @name Private member functions
        /// @{

        private:

            virtual arma::Mat<Complex> calcY() const override;

        /// @}

        private:

            Complex nomRatio_; ///< Nominal voltage ratio, V_nom,p/V_nom,s
            Complex offNomRatio10_; ///< Off nominal mulitplier for nomRatio_, winding between phases 1 and 0.
            Complex offNomRatio12_; ///< Off nominal mulitplier for nomRatio_, winding between phases 1 and 2.
            Complex YL10_; ///< Series leakage admittance, phases 1 and 0.
            Complex YL12_; ///< Series leakage admittance, phases 1 and 2.
            Complex YTie_; ///< Admittance tying terminals 2 between primary and secondary.
            Complex YGround_; ///< Admittance tying secondary terminal 2 to ground.
    };
}

#endif // VV_TRANSFORMER_DOT_H
