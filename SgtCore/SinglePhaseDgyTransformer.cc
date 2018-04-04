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

#include "SinglePhaseDgyTransformer.h"

#include<SgtCore/Bus.h>

using namespace arma;
using namespace std;

namespace Sgt
{
    Col<Complex> SinglePhaseDgyTransformer::VWindings0() const
    {
        Col<Complex> V = mapPhases(VBus()[0], bus0()->phases(), phases0());
        return {{V(0) - V(1)}};
    }
    Col<Complex> SinglePhaseDgyTransformer::VWindings1() const
    {
        Col<Complex> V = mapPhases(VBus()[1], bus1()->phases(), phases1());
        return {{V(0)}};
    }

    Col<Complex> SinglePhaseDgyTransformer::IWindings0() const
    {
        constexpr double c = 1.0 / 3.0;
        Col<Complex> Ibi = -IBus()[0]; 
        Col<Complex> I = mapPhases(Ibi, bus0()->phases(), phases0());
        return {{c * (I(0) - I(1))}};
    }
    Col<Complex> SinglePhaseDgyTransformer::IWindings1() const
    {
        Col<Complex> Ibi = -IBus()[1]; 
        Col<Complex> I = mapPhases(Ibi, bus1()->phases(), phases1());
        return {{I(0)}};
    }

    Mat<Complex> SinglePhaseDgyTransformer::calcY() const
    {
        Complex a = turnsRatio()(0);
        Complex YL = 1.0 / ZL()(0);

        Complex ai = 1.0 / a;
        Complex aci = conj(ai);
        Complex a2i = ai * aci;

        Mat<Complex> result(
            {
            {a2i,  -a2i,  -aci},
            {-a2i, a2i,  aci},
            {-ai,    ai,   1.0},
            });
        result *= YL;
        return result;
    }
};
