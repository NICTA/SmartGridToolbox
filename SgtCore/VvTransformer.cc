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

#include "VvTransformer.h"

using namespace arma;
using namespace std;

namespace Sgt
{
    template<size_t i> Col<Complex> VWindings(const BranchAbc& b)
    {
        Col<Complex> V = b.VBus()[i];
        return {{V(0) - V(1), V(1) - V(2)}};
    }
    Col<Complex> VvTransformer::VWindings0() const
    {
        return VWindings<0>(*this);
    }
    Col<Complex> VvTransformer::VWindings1() const
    {
        return VWindings<1>(*this);
    }

    template<size_t i> Col<Complex> IWindings(const BranchAbc& b)
    {
        Col<Complex> I = b.IBus()[i];
        return {{I(0), -I(2)}};
    }
    Col<Complex> VvTransformer::IWindings0() const
    {
        return IWindings<0>(*this);
    }
    Col<Complex> VvTransformer::IWindings1() const
    {
        return IWindings<1>(*this);
    }
            
    Mat<Complex> VvTransformer::calcY() const
    {
        // 0-1 windings:
        Complex a = 1.0 / turnsRatio()(0);
        Complex ac = conj(a);
        Complex a2 = a * ac;

        Complex ylw1 = 1.0 / ZL()(0);
        a *= ylw1;
        ac *= ylw1;
        a2 *= ylw1;
        
        // 1-2 windings:
        Complex b = 1.0 / turnsRatio()(1);
        Complex bc = conj(b);
        Complex b2 = b * bc;

        Complex ylw2 = 1.0 / ZL()(1);
        b *= ylw2;
        bc *= ylw2;
        b2 *= ylw2;

        return
        { 
        {a2,                -a2,                0.0,                -ac,                ac,                 0.0},
        {-a2,               a2+b2-ac-bc,        -b2,                ac,                 0.0,                bc},
        {0.0,               -b2,                b2,                 0.0,                bc,                 -bc},
        {-a,                a,                  0.0,                ylw1,               -ylw1,              0.0},
        {a,                 0.0,                b,                  -ylw1,              ylw1+ylw2-a-b,      -ylw2},
        {0.0,               b,                  -b,                 0.0,                -ylw2,              ylw2}
        };
    }
}
