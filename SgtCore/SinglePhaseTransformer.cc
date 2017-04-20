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

#include "SinglePhaseTransformer.h"

using namespace arma;
using namespace std;

namespace Sgt
{
    template<size_t i> Col<Complex> VWindings(const BranchAbc& b)
    {
        Col<Complex> V = b.VBus()[i];
        return {{V(0)}};
    }
    Col<Complex> SinglePhaseTransformer::VWindings0() const
    {
        return VWindings<0>(*this);
    }
    Col<Complex> SinglePhaseTransformer::VWindings1() const
    {
        return VWindings<1>(*this);
    }

    template<size_t i> Col<Complex> IWindings(const BranchAbc& b)
    {
        Col<Complex> I = -b.IBusInj()[i];
        return {{I(0)}};
    }
    Col<Complex> SinglePhaseTransformer::IWindings0() const
    {
        return IWindings<0>(*this);
    }
    Col<Complex> SinglePhaseTransformer::IWindings1() const
    {
        return IWindings<1>(*this);
    }

    Mat<Complex> SinglePhaseTransformer::calcY() const
    {
        Complex a = turnsRatio()(0);
        Complex YL = 1.0 / ZL()(0);

        Complex ai = 1.0 / a;
        Complex aci = conj(ai);
        Complex a2i = ai * aci;
        return
            {
            {YL * a2i, -YL * aci},
            {-YL * ai, YL}
            };
    }
};
