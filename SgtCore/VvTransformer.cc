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

#include "VvTransformer.h"

using namespace arma;
using namespace std;

namespace Sgt
{
    template<size_t i> Col<Complex> VWindings(const BranchAbc& b)
    {
        Col<Complex> V = b.VBus()[i];
        return {{V(0) - V(1), V(2) - V(1)}};
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
        Col<Complex> I = -b.IBusInj()[i];
        return {{I(0), I(2)}};
    }
    Col<Complex> VvTransformer::IWindings0() const
    {
        return IWindings<0>(*this);
    }
    Col<Complex> VvTransformer::IWindings1() const
    {
        return IWindings<1>(*this);
    }
            
    void VvTransformer::setYTie(Complex YTie)
    {
        YTie_ = YTie;
        invalidate();
    }
            
    void VvTransformer::setYGround(Complex YGround)
    {
        YGround_ = YGround;
        invalidate();
    }

    Mat<Complex> VvTransformer::calcY() const
    {
        // 0-1 windings:
        Complex a = 1.0 / turnsRatio()(0);
        Complex ac = conj(a);
        Complex a2 = a * ac;

        Complex YL10 = 1.0 / ZL()(0);
        a *= YL10;
        ac *= YL10;
        a2 *= YL10;
        
        // 1-2 windings:
        Complex b = 1.0 / turnsRatio()(1);
        Complex bc = conj(b);
        Complex b2 = b * bc;

        Complex YL12 = 1.0 / ZL()(1);
        b *= YL12;
        bc *= YL12;
        b2 *= YL12;

        return
        { 
        {a2,                -a2,                0.0,                -ac,                ac,                 0.0},
        {-a2,               a2+b2+YTie_,        -b2,                ac,                 -ac-bc-YTie_,       bc},
        {0.0,               -b2,                b2,                 0.0,                bc,                 -bc},
        {-a,                a,                  0.0,                YL10,               -YL10,              0.0},
        {a,                 -a-b-YTie_,         b,                  -YL10,    YL10+YL12+YTie_+YGround_,     -YL12},
        {0.0,               b,                  -b,                 0.0,                -YL12,              YL12}
        };
    }
}
