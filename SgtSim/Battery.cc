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

#include "Battery.h"

#include <iostream>

using namespace std;

namespace Sgt
{
    void Battery::updateState(Time t)
    {
        double dt = lastUpdated() == posix_time::neg_infin ? 0 : dSeconds(t - lastUpdated());
        if (dt > 0)
        {
            charge_ += internalPower() * dSeconds(t - lastUpdated()) / 3600.0; // Charge in MWh.
            if (charge_ < 0.0) charge_ = 0.0;
        }
    }

    double Battery::PDc() const
    {
        double result = 0.0;
        if ((requestedPower_ > 0 && charge_ < maxCharge_) || (requestedPower_ < 0 && charge_ > 0))
        {
            result = requestedPower_ < 0
                     ? std::max(requestedPower_, -maxDischargePower_)
                     : std::min(requestedPower_, maxChargePower_);
        }
        return result;
    }

    double Battery::internalPower()
    {
        double P = PDc();
        return (P > 0 ? P * chargeEfficiency_ : P / dischargeEfficiency_);
    }
}
