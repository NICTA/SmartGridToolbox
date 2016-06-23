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

#ifndef YY_TRANSFORMER_DOT_H
#define YY_TRANSFORMER_DOT_H

#include <SgtCore/Branch.h>

namespace Sgt
{
    /// @brief Wye-wye transformer.
    ///
    /// Equivalent to a single phase transformer on each phase.
    /// @ingroup PowerFlowCore
    class YyTransformer : public BranchAbc
    {
        public:

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("yy_transformer");
                return result;
            }

        /// @}

        /// @name Lifecycle
        /// @{

            /// @brief Constructor
            /// @param a The complex turns ratio (not voltage ratio) for each of the six windings.
            /// @param ZL The leakage impedance, must be > 0.
            YyTransformer(const std::string& id, Complex a, Complex ZL, Complex YM,
                    const Phases& phases0 = {Phase::A, Phase::B, Phase::C},
                    const Phases& phases1 = {Phase::A, Phase::B, Phase::C}) :
                Component(id),
                BranchAbc(phases0, phases1),
                a_(a),
                YL_(1.0 / ZL),
                YM_(YM)
            {
                // Empty.
            }

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

            Complex a()
            {
                return a_;
            }

            void set_a(Complex a)
            {
                a_ = a;
                isValid_ = false;
            }

            Complex ZL()
            {
                return 1.0 / YL_;
            }

            void setZL(Complex ZL)
            {
                YL_ = 1.0 / ZL;
                isValid_ = false;
            }

            Complex YM()
            {
                return YM_;
            }

            void setYM(Complex YM)
            {
                YM_ = YM;
                isValid_ = false;
            }

        /// @}

        /// @name Private member functions
        /// @{

        private:

            void validate() const;

        /// @}

        private:
            Complex a_;  ///< Complex turns ratio, n0/n1.
            Complex YL_; ///< Series leakage admittance.
            Complex YM_; ///< Shunt magnetising impedance.

            mutable bool isValid_{false};
            mutable arma::Mat<Complex> Y_;
    };
}

#endif // YY_TRANSFORMER_DOT_H
