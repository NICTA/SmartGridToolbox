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
        /// @param nomRatioW1 Nominal complex voltage/turns ratio for winding between phases 1 and 0.
        /// @param nomRatioW2 Nominal complex voltage/turns ratio for winding between phases 1 and 2.
        /// @param offNomRatioW1 Off-nominal complex voltage/turns ratio for winding between phases 1 and 0.
        /// @param offNomRatioW2 Off-nominal complex voltage/turns ratio for winding between phases 1 and 2.
        /// @param ZLW1 The leakage impedance between phases 1 and 0, must be > 0.
        /// @param ZLW2 The leakage impedance between phases 1 and 2, must be > 0.
        /// @param phases0 The phases on the first (primary) side.
        /// @param phases1 The phases on the second (secondary) side.
        VvTransformer(const std::string& id, Complex nomRatioW1, Complex nomRatioW2,
                Complex offNomRatioW1, Complex offNomRatioW2,
                Complex ZLW1, Complex ZLW2,
                const Phases& phases0 = {Phase::A, Phase::B, Phase::C},
                const Phases& phases1 = {Phase::A, Phase::B, Phase::C}) :
            Component(id),
            TransformerAbc(phases0, phases1, {nomRatioW1, nomRatioW2}, {offNomRatioW1, offNomRatioW2},
                    {ZLW1, ZLW2}, {})
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

        /// @name Private member functions
        /// @{

        private:

        virtual arma::Mat<Complex> calcY() const override;

        /// @}

        private:
    };
}

#endif // VV_TRANSFORMER_DOT_H
