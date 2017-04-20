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
    void VvTransformer::setNomRatio(Complex nomRatio)
    {
        nomRatio_ = nomRatio;
        isValid_ = false;
        admittanceChanged().trigger();
    }
            
    void VvTransformer::setOffNomRatio10(Complex offNomRatio10)
    {
        offNomRatio10_ = offNomRatio10;
        isValid_ = false;
        admittanceChanged().trigger();
    }
            
    void VvTransformer::setOffNomRatio12(Complex offNomRatio12)
    {
        offNomRatio12_ = offNomRatio12;
        isValid_ = false;
        admittanceChanged().trigger();
    }
            
    void VvTransformer::setZL10(Complex ZL10)
    {
        YL10_ = 1.0 / ZL10;
        isValid_ = false;
        admittanceChanged().trigger();
    }

    void VvTransformer::setZL12(Complex ZL12)
    {
        YL12_ = 1.0 / ZL12;
        isValid_ = false;
        admittanceChanged().trigger();
    }
            
    void VvTransformer::setYTie(Complex YTie)
    {
        YTie_ = YTie;
        admittanceChanged().trigger();
    }
            
    void VvTransformer::setYGround(Complex YGround)
    {
        YGround_ = YGround;
        admittanceChanged().trigger();
    }

    Mat<Complex> VvTransformer::inServiceY() const
    {
        if (!isValid_)
        {
            validate();
        }
        return Y_;
    }

    void VvTransformer::validate() const
    {
        // 2-1 windings:
        Complex a = 1.0 / a10();
        Complex ac = conj(a);
        Complex a2 = a * ac;
        a *= YL10_;
        ac *= YL10_;
        a2 *= YL10_;
        
        // 2-3 windings:
        Complex b = 1.0 / a12();
        Complex bc = conj(b);
        Complex b2 = b * bc;
        b *= YL12_;
        bc *= YL12_;
        b2 *= YL12_;

        Y_ = 
        { 
            {a2,                -a2,                0.0,                -ac,                ac,                 0.0},
            {-a2,               a2+b2+YTie_,        -b2,                ac,                 -ac-bc-YTie_,       bc},
            {0.0,               -b2,                b2,                 0.0,                bc,                 -bc},
            {-a,                a,                  0.0,                YL10_,              -YL10_,             0.0},
            {a,                 -a-b-YTie_,         b,                  -YL10_,   YL10_+YL12_+YTie_+YGround_,   -YL12_},
            {0.0,               b,                  -b,                 0.0,                -YL12_,             YL12_}
        };
    }
}
