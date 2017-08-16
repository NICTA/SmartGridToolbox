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
        double prevPDc = PDc_;
        PDc_ = calcPDc();
        if (std::abs(prevPDc - PDc_) > std::numeric_limits<double>::epsilon())
        {
            dcPowerChanged().trigger();
        }

        double dt = lastUpdated() == TimeSpecialValues::neg_infin ? 0 : dSeconds(t - lastUpdated());
        if (dt > 0)
        {
            charge_ -= internalPower() * dSeconds(t - lastUpdated()) / 3600.0; // Charge in MWh.
            if (charge_ < 0.0) charge_ = 0.0;
            if (charge_ > maxCharge_) charge_ = maxCharge_;
        }
        dcPowerChanged().trigger();
    }
    
    void Battery::setMaxCharge(double val)
    {
        maxCharge_ = val;
        needsUpdate().trigger();
    }
    
    void Battery::setMaxChargePower(double val)
    {
        maxChargePower_ = val;
        needsUpdate().trigger();
    }
    
    void Battery::setMaxDischargePower(double val)
    {
        maxDischargePower_ = val;
        needsUpdate().trigger();
    }

    void Battery::setChargeEfficiency(double val)
    {
        chargeEfficiency_ = val;
        needsUpdate().trigger();
    }

    void Battery::setDischargeEfficiency(double val)
    {
        dischargeEfficiency_ = val;
        needsUpdate().trigger();
    }

    void Battery::setRequestedPower(double val)
    {
        requestedPower_ = val;
        needsUpdate().trigger();
    } 

    double Battery::internalPower()
    {
        double P = PDc();
        return (P > 0 ? P / dischargeEfficiency_ : P * chargeEfficiency_);
    }

    double Battery::calcPDc() const
    {
        double result = 0.0;
        if (requestedPower_ > 0 && charge_ > 0)
        {
            // Discharging.
            result = std::min(requestedPower_, maxDischargePower_);
        }
        else if (requestedPower_ < 0 && charge_ < maxCharge_)
        {
            // Charging.
            result = std::max(requestedPower_, -maxChargePower_);
        }
        return result;
    }
}
