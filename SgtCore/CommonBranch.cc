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

#include "CommonBranch.h"

namespace Sgt
{
    arma::Mat<Complex> CommonBranch::inServiceY() const
    {
        if (!isValid_)
        {
            validate();
        }
        return Y_;
    }

    json CommonBranch::toJson() const
    {
        json j = BranchAbc::toJson();
        j[sComponentType()] = {
            {"tap_ratio_magnitude", std::abs(tapRatio_)},
            {"tap_ratio_angle_deg", std::arg(tapRatio_) * 180 / pi},
            {"Y_series", YSeries_},
            {"Y_shunt", YShunt_},
            {"rate_A", rateA_},
            {"rate_B", rateB_},
            {"rate_C", rateC_}};
        return j;
    }

    void CommonBranch::validate() const
    {
        if (!isValid_)
        {
            Y_ = arma::Mat<Complex>(2, 2, arma::fill::none);
            Complex c1 = YSeries_ + 0.5 * YShunt_;
            Y_(0, 0) = c1 / norm(tapRatio_); // Yes, C++ does define norm(c) as |c|^2 - field norm?
            Y_(0, 1) = -YSeries_ / conj(tapRatio_);
            Y_(1, 0) = -YSeries_ / tapRatio_;
            Y_(1, 1) = c1;

            isValid_ = true;
        }
    }
}
