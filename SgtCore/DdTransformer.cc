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

#include "DdTransformer.h"

namespace Sgt
{
    arma::Mat<Complex> DdTransformer::inServiceY() const
    {
        if (!isValid_)
        {
            validate();
        }
        return Y_;
    }
            
    void DdTransformer::setZL(Complex ZL)
    {
        YL_ = 1.0 / ZL;
        isValid_ = false;
        admittanceChanged().trigger();
    }
            
    void DdTransformer::setYM(Complex YM)
    {
        YM_ = YM;
        isValid_ = false;
        admittanceChanged().trigger();
    }

    void DdTransformer::validate() const
    {
        Complex ai = 1.0 / a();
        Complex aci = conj(ai);
        Complex a2i = ai * aci;

        Complex data[] =  {
            2.0 * a2i, -a2i, -a2i, -2.0 * aci, aci, aci,
            -a2i, 2.0 * a2i, -a2i, aci, -2.0 * aci, aci,
            -a2i, -a2i, 2.0 * a2i, aci, aci, -2.0 * aci,
            -2.0 * ai, ai, ai, 2.0, -1.0, -1.0,
            ai, -2.0 * ai, ai, -1.0, 2.0, -1.0,
            ai, ai, -2.0 * ai, -1.0, -1.0, 2.0};
        
        Y_ = arma::Mat<Complex>(6, 6, arma::fill::none);
        for (arma::uword i = 0; i < 6; ++i)
        {
            for (arma::uword j = 0; j < 6; ++j)
            {
                Y_(i, j) = YL_ * data[6 * i + j];
            }
        }
    }
};
