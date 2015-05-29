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

namespace Sgt
{
    arma::Mat<Complex> YyTransformer::inServiceY() const
    {
        if (!isValid_)
        {
            validate();
        }
        return Y_;
    }

    void YyTransformer::validate() const
    {
        auto n = phases0().size();
        Y_ = arma::Mat<Complex>(2 * n, 2 * n, arma::fill::zeros);
        for (arma::uword i = 0; i < n; ++i)
        {
            Y_(i, i) = (YL_ + YM_) / (a_ * conj(a_));
            Y_(i, i + n) = -YL_ / conj(a_);
            Y_(i + n, i) = -YL_ / a_;
            Y_(i + n, i + n) = YL_;
        }
    }
};
