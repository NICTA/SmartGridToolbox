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
    /// @brief Delta-grounded wye transformer.
    /// @ingroup PowerFlowCore
    class VvTransformer : public BranchAbc
    {
        public:

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("odod_transformer");
                return result;
            }

        /// @}

        /// @name Lifecycle
        /// @{

            /// @brief Constructor
            /// @param nomRatio Nominal complex voltage/turns ratio for windings.
            /// @param offNomRatio13 Off-nominal complex voltage/turns ratio for windings between phases 1 and 3.
            /// @param offNomRatio23 Off-nominal complex voltage/turns ratio for windings between phases 2 and 3.
            /// @param ZL The leakage impedance, must be > 0.
            /// @param phases0 The phases on the first (primary) side.
            /// @param phases1 The phases on the second (secondary) side.
            VvTransformer(const std::string& id, Complex nomRatio,
                    Complex offNomRatio13, Complex offNomRatio23, Complex ZL, 
                    const Phases& phases0 = {Phase::A, Phase::B, Phase::C},
                    const Phases& phases1 = {Phase::A, Phase::B, Phase::C}) :
                Component(id),
                BranchAbc(phases0, phases1),
                nomRatio_(nomRatio),
                offNomRatio13_(offNomRatio13),
                offNomRatio23_(offNomRatio23),
                YL_(1.0/ZL)
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

            void setNomRatio(Complex nomRatio)
            {
                nomRatio_ = nomRatio;
                isValid_ = false;
            }

            Complex offNomRatio13() const
            {
                return offNomRatio13_;
            }

            void setOffNomRatio13(Complex offNomRatio13)
            {
                offNomRatio13_ = offNomRatio13;
                isValid_ = false;
            }

            Complex offNomRatio23() const
            {
                return offNomRatio23_;
            }

            void setOffNomRatio23(Complex offNomRatio23)
            {
                offNomRatio23_ = offNomRatio23;
                isValid_ = false;
            }

            Complex a13() const
            {
                return offNomRatio13_ * nomRatio_;
            }

            Complex a23() const
            {
                return offNomRatio23_ * nomRatio_;
            }

            Complex ZL() const
            {
                return 1.0 / YL_;
            }

            void setZL(Complex ZL)
            {
                YL_ = 1.0 / ZL;
                isValid_ = false;
            }

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
            Complex offNomRatio13_; ///< Off nominal mulitplier for nomRatio_, winding between phases 1 and 3.
            Complex offNomRatio23_; ///< Off nominal mulitplier for nomRatio_, winding between phases 2 and 3.
            Complex YL_; ///< Series leakage admittance.

            mutable bool isValid_{false};
            mutable arma::Mat<Complex> Y_;
    };
}

#endif // VV_TRANSFORMER_DOT_H
