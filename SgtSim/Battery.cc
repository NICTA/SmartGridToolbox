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

#include "Battery.h"

#include <iostream>

using namespace std;

namespace Sgt
{
    void Battery::updateState(const Time& t)
    {
        double dt = lastUpdated() == TimeSpecialValues::neg_infin ? 0 : dSeconds(t - lastUpdated());
        sgtLogDebug(LogLevel::VERBOSE) << "Battery updateState(:): dt = " << dt << endl;
        if (dt > 0)
        {
            auto internalPow = internalPower();
            sgtLogDebug(LogLevel::VERBOSE) << "Battery updateState(:): internal power = " << internalPow << endl;
            sgtLogDebug(LogLevel::VERBOSE) << "Battery updateState(:): old soc = " << soc_ << endl;
            // Euler step makes repeated updates of battery easier to handle.
            soc_ -= internalPow * dt / 3600.0; // Charge in MWh.
            if (soc_ < 0.0) soc_ = 0.0;
            if (soc_ > maxSoc_) soc_ = maxSoc_;
            sgtLogDebug(LogLevel::VERBOSE) << "Battery updateState(:): new soc = " << soc_ << endl;
        }

        double prevRequestedPDc = requestedPDc_;
        requestedPDc_ = calcRequestedPDc();
        if (abs(prevRequestedPDc - requestedPDc_) > numeric_limits<double>::epsilon())
        {
            sgtLogDebug(LogLevel::VERBOSE) << "Battery updateState(:): trigger dcPowerChanged()" << endl;
            dcPowerChanged().trigger();
        }
    }
    
    void Battery::setMaxSoc(double val)
    {
        maxSoc_ = val;
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

    double Battery::internalPower() const
    {
        double P = actualPDc();
        return (P > 0 ? P / dischargeEfficiency_ : P * chargeEfficiency_);
    }

    double Battery::calcRequestedPDc() const
    {
        double result = 0.0;
        if (requestedPower_ > 0 && soc_ > 0)
        {
            // Discharging.
            result = min(requestedPower_, maxDischargePower_);
        }
        else if (requestedPower_ < 0 && soc_ < maxSoc_)
        {
            // Charging.
            result = max(requestedPower_, -maxChargePower_);
        }
        sgtLogDebug(LogLevel::VERBOSE) << "Battery calcRequestedPDc(): requested power = " << requestedPower_ << endl;
        sgtLogDebug(LogLevel::VERBOSE) << "Battery calcRequestedPDc(): soc = " << soc_ << endl;
        sgtLogDebug(LogLevel::VERBOSE) << "Battery calcRequestedPDc(): max SOC = " << maxSoc_ << endl;
        sgtLogDebug(LogLevel::VERBOSE) << "Battery calcRequestedPDc(): result = " << result << endl;
        return result;
    }
}
