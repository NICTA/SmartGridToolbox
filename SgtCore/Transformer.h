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

            TransformerAbc(const Phases& phases0, const Phases& phases1) : BranchAbc(phases0, phases1) {}

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            /// @brief Transformer type.
            virtual TransformerType transformerType() const = 0;

            /// @brief Voltages across primary windings.
            virtual arma::Col<Complex> VWindings0() const = 0;

            /// @brief Voltages across secondary windings.
            virtual arma::Col<Complex> VWindings1() const = 0;
            
            /// @brief Load currents across primary windings.
            ///
            /// Excludes any circular currents that may exist.
            virtual arma::Col<Complex> IWindings0() const = 0;

            /// @brief Load currents across secondary windings.
            ///
            /// Excludes any circular currents that may exist.
            virtual arma::Col<Complex> IWindings1() const = 0;
    };
}

#endif // TRANSFORMER_DOT_H
