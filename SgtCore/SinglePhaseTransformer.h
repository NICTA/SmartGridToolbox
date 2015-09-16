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

#ifndef SINGLE_PHASE_TRANSFORMER_DOT_H
#define SINGLE_PHASE_TRANSFORMER_DOT_H

#include <SgtCore/Branch.h>

namespace Sgt
{
    /// @brief Single phase transformer.
    /// @ingroup PowerFlowCore
    class SinglePhaseTransformer : public BranchAbc
    {
        public:

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("single_phase_transformer");
                return result;
            }

        /// @}

            SGT_PROPS_INIT(SinglePhaseTransformer);
            SGT_PROPS_INHERIT(SinglePhaseTransformer, Component);
            SGT_PROPS_INHERIT(SinglePhaseTransformer, BranchAbc);

        /// @name Lifecycle
        /// @{

            /// @brief Constructor
            /// @param nomVRatio The complex voltage ratio for each of the six windings.
            /// @param offNomRatio The off nominal ratio for each of the six windings.
            /// @param ZL The leakage impedance, must be > 0.
            SinglePhaseTransformer(const std::string& id, Complex nomVRatio, Complex offNomRatio, Complex ZL) :
                Component(id),
                BranchAbc(Phase::BAL, Phase::BAL), nomVRatio_(nomVRatio), offNomRatio_(offNomRatio), YL_(1.0/ZL)
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

            // virtual void print(std::ostream& os) const override; // TODO

        /// @}

        /// @name Overridden from BranchAbc:
        /// @{

            virtual arma::Mat<Complex> inServiceY() const override;

        /// @}

        /// @name Parameters:
        /// @{

            Complex nomVRatio() const
            {
                return nomVRatio_;
            }

            void setNomVRatio(Complex nomVRatio)
            {
                nomVRatio_ = nomVRatio;
                isValid_ = false;
            }

            Complex offNomRatio() const
            {
                return offNomRatio_;
            }

            void setOffNomRatio(Complex offNomRatio)
            {
                offNomRatio_ = offNomRatio;
                isValid_ = false;
            }

            Complex a() const
            {
                return offNomRatio_ * nomVRatio_;
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

        /// @name Private member functions
        /// @{

        private:

            void validate() const;

        /// @}

        private:

            Complex nomVRatio_; ///< Nominal voltage ratio.
            Complex offNomRatio_; ///< Off nominal complex turns ratio.
            Complex YL_; ///< Series leakage admittance.

            mutable bool isValid_{false};
            mutable arma::Mat<Complex> Y_;
    };
}

#endif // SINGLE_PHASE_TRANSFORMER_DOT_H
