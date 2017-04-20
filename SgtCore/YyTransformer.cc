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

#include "YyTransformer.h"

using namespace arma;
using namespace std;

namespace Sgt
{
    template<size_t i> Col<Complex> VWindings(const BranchAbc& b)
    {
        Col<Complex> V = b.VBus()[i];
        return {{V(0), V(1), V(2)}};
    }
    Col<Complex> YyTransformer::VWindings0() const
    {
        return VWindings<0>(*this);
    }
    Col<Complex> YyTransformer::VWindings1() const
    {
        return VWindings<1>(*this);
    }

    template<size_t i> Col<Complex> IWindings(const BranchAbc& b)
    {
        Col<Complex> I = -b.IBusInj()[i];
        return {{I(0), I(1), I(2)}};
    }
    Col<Complex> YyTransformer::IWindings0() const
    {
        return IWindings<0>(*this);
    }
    Col<Complex> YyTransformer::IWindings1() const
    {
        return IWindings<1>(*this);
    }

    void YyTransformer::set_a(Complex a)
    {
        a_ = a;
        invalidate();
    }
            
    void YyTransformer::setZL(Complex ZL)
    {
        YL_ = 1.0 / ZL;
        invalidate();
    }
            
    void YyTransformer::setYM(Complex YM)
    {
        YM_ = YM;
        invalidate();
    }

    Mat<Complex> YyTransformer::calcY() const
    {
        auto n = phases0().size();
        Mat<Complex> result(2 * n, 2 * n, fill::zeros);
        for (uword i = 0; i < n; ++i)
        {
            result(i, i) = (YL_ + YM_) / (a_ * conj(a_));
            result(i, i + n) = -YL_ / conj(a_);
            result(i + n, i) = -YL_ / a_;
            result(i + n, i + n) = YL_;
        }
        return result;
    }
};
