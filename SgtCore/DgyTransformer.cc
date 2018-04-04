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

#include "DgyTransformer.h"

using namespace arma;
using namespace std;

namespace Sgt
{
    Col<Complex> DgyTransformer::VWindings0() const
    {
        Col<Complex> V = VBus()[0];
        return {{V(0) - V(1), V(1) - V(2), V(2) - V(0)}};
    }
    Col<Complex> DgyTransformer::VWindings1() const
    {
        Col<Complex> V = VBus()[1];
        return {{V(0), V(1), V(2)}};
    }

    Col<Complex> DgyTransformer::IWindings0() const
    {
        constexpr double c = 1.0 / 3.0;
        Col<Complex> I = -IBus()[0];
        return {{c * (I(0) - I(1)), c * (I(1) - I(2)), c * (I(2) - I(0))}};
    }
    Col<Complex> DgyTransformer::IWindings1() const
    {
        Col<Complex> I = -IBus()[1];
        return {{I(0), I(1), I(2)}};
    }

    Mat<Complex> DgyTransformer::calcY() const
    {
        Complex a = turnsRatio()(0);
        Complex YL = 1.0 / ZL()(0);

        Complex ai = 1.0 / a;
        Complex aci = conj(ai);
        Complex a2i = ai * aci;

        Mat<Complex> result(
            {
            {2 * a2i,  -a2i,  -a2i, -aci,  0.0,  aci},
            {-a2i, 2 * a2i,  -a2i,  aci, -aci,  0.0},
            {-a2i,  -a2i, 2 * a2i,  0.0,  aci, -aci},
            {-ai,    ai,   0.0,  1.0,  0.0,  0.0},
            {0.0,   -ai,    ai,  0.0,  1.0,  0.0},
            {ai,   0.0,   -ai,  0.0,  0.0,  1.0}
            });
        result *= YL;
        return result;
    }
};
