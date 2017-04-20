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

#ifndef DGY_TRANSFORMER_DOT_H
#define DGY_TRANSFORMER_DOT_H

#include <SgtCore/Transformer.h>

namespace Sgt
{
    /// @brief Delta-grounded wye transformer.
    /// @ingroup PowerFlowCore
    class DgyTransformer : public TransformerAbc
    {
        public:

            SGT_PROPS_INIT(DgyTransformer);
            SGT_PROPS_INHERIT(TransformerAbc);

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("dgy_transformer");
                return result;
            }

        /// @}

        /// @name Lifecycle
        /// @{

            /// @brief Constructor
            /// @param nomVRatioDY Nominal complex voltage ratio (not turns ratio) for each pair of windings.
            /// @param offNomRatioDY Off nominal complex ratio for each of the six windings.
            /// @param ZL The leakage impedance, must be > 0.
            DgyTransformer(const std::string& id, Complex nomVRatioDY, Complex offNomRatioDY, Complex ZL,
                    const Phases& phases0 = {Phase::A, Phase::B, Phase::C},
                    const Phases& phases1 = {Phase::A, Phase::B, Phase::C}) :
                Component(id),
                TransformerAbc(phases0, phases1),
                nomVRatioDY_(nomVRatioDY),
                offNomRatioDY_(offNomRatioDY),
                YL_(1.0/ZL)
            {
                // Empty.
            }

        /// @}
        
        /// @name Overridden from TransformerAbc:
        /// @{
            
            virtual TransformerType transformerType() const override {return TransformerType::DGY;}
            virtual arma::Col<Complex> VWindings0() const override;
            virtual arma::Col<Complex> VWindings1() const override;
            virtual arma::Col<Complex> IWindings0() const override;
            virtual arma::Col<Complex> IWindings1() const override;

        /// @}

        /// @name Overridden from BranchAbc:
        /// @{

            virtual arma::Mat<Complex> inServiceY() const override;

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

            Complex nomVRatioDY() const
            {
                return nomVRatioDY_;
            }

            void setNomVRatioDY(Complex nomVRatioDY);

            Complex offNomRatioDY() const
            {
                return offNomRatioDY_;
            }

            void setOffNomRatioDY(Complex offNomRatioDY);

            Complex a() const
            {
                return offNomRatioDY_ * nomVRatioDY_;
            }

            Complex ZL() const
            {
                return 1.0 / YL_;
            }

            void setZL(Complex ZL);

        /// @}

        /// @name Private member functions
        /// @{

        private:

            void validate() const;

        /// @}

        private:

            Complex nomVRatioDY_; ///< Nominal voltage ratio, V_D / V_Y where V_D is phase-phase & V_Y is phase-ground.
            Complex offNomRatioDY_; ///< Off nominal complex turns ratio.
            Complex YL_; ///< Series leakage admittance.

            mutable bool isValid_{false};
            mutable arma::Mat<Complex> Y_;
    };
}

#endif // DGY_TRANSFORMER_DOT_H
