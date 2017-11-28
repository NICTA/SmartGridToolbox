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

#include <SgtCore/Transformer.h>

namespace Sgt
{
    /// @brief Single phase transformer.
    /// @ingroup PowerFlowCore
    class SinglePhaseTransformer : public TransformerAbc
    {
        public:

        SGT_PROPS_INIT(SinglePhaseTransformer);
        SGT_PROPS_INHERIT(TransformerAbc);

        /// @name Static member functions:
        /// @{

        static const std::string& sComponentType()
        {
            static std::string result("single_phase_transformer");
            return result;
        }

        /// @}

        /// @name Lifecycle
        /// @{

        /// @brief Constructor
        /// @param nomTurnsRatio The complex turns ratio for each of the three winding pairs.
        /// @param offNomRatio The off nominal ratio for each of the three winding pairs.
        /// @param ZL The leakage impedance, must be > 0.
        SinglePhaseTransformer(const std::string& id, Complex nomTurnsRatio, Complex offNomRatio, Complex ZL,
                const Phase phase0 = Phase::BAL, const Phase phase1 = Phase::BAL) :
            Component(id),
            TransformerAbc({phase0}, {phase1}, {nomTurnsRatio}, {1.0}, {ZL}, {})
        {
            // Empty.
        }

        /// @}

        /// @name Overridden from TransformerAbc:
        /// @{

        virtual TransformerType transformerType() const override {return TransformerType::SINGLE_PHASE;}
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

#endif // SINGLE_PHASE_TRANSFORMER_DOT_H
