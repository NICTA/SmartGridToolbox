// Copyright 2015-2016 National ICT Australia Limited (NICTA)
// Copyright 2016-2019 The Australian National University
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

#ifndef SINGLE_PHASE_DGY_TRANSFORMER_DOT_H
#define SINGLE_PHASE_DGY_TRANSFORMER_DOT_H

#include <SgtCore/Transformer.h>

namespace Sgt
{
    /// @brief Delta-grounded wye transformer.
    /// @ingroup PowerFlowCore
    class SinglePhaseDgyTransformer : public TransformerAbc
    {
        public:

        SGT_PROPS_INIT(SinglePhaseDgyTransformer);
        SGT_PROPS_INHERIT(TransformerAbc);

        /// @name Static member functions:
        /// @{

        static const std::string& sComponentType()
        {
            static std::string result("single_phase_dgy_transformer");
            return result;
        }

        /// @}

        /// @name Lifecycle
        /// @{

        /// @brief Constructor
        /// @param nomTurnsRatio Nominal turns ratio for each pair of windings = V_delta_0 / V_wye_1.
        /// @param offNomRatio Off nominal complex ratio for each of the six windings.
        /// @param ZL The leakage impedance, must be > 0.
        SinglePhaseDgyTransformer(const std::string& id, Complex nomTurnsRatio, Complex offNomRatio, Complex ZL,
                const Phases& phases0, const Phase& phase1) :
            Component(id),
            TransformerAbc(phases0, {phase1}, {nomTurnsRatio}, {offNomRatio}, {ZL}, {})
        {
            // Empty.
        }

        /// @}

        /// @name Overridden from TransformerAbc:
        /// @{

        virtual TransformerType transformerType() const override {return TransformerType::SINGLE_PHASE_DGY;}
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

        /// @name Private member functions
        /// @{

        private:

        virtual arma::Mat<Complex> calcY() const override;

        /// @}
    };
}

#endif // SINGLE_PHASE_DGY_TRANSFORMER_DOT_H
