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

#include "SimpleBuilding.h"

namespace
{
    double propTbNorm(double dt, double Tb0, double Ts, double kb,
                      double kh, double Cb)
    {
        // Solve dTb = a(Ts - T), with a defined below.
        double a = (kb + kh) / Cb;
        double Tb1 = Ts + exp(-a * dt) * (Tb0 - Ts);
        return Tb1;
    }

    double propTbMaxed(double dt, double Tb0, double dQg0, double dQg1,
                       double Te0, double Te1, double kb, double dQh, double Cb)
    {
        // Solve dTb = a * T + b * t + c, with a, b, c defined below.
        double Tb1;
        if (std::abs(dt) < std::numeric_limits<double>::epsilon())
        {
            Tb1 = Tb0;
        }
        else
        {
            double a = -kb / Cb;
            double a2 = a * a;
            double b = ((dQg1 - dQg0) / dt + kb * (Te1-Te0) / dt) / Cb;
            double c = (dQg0 + kb * Te0 + dQh) / Cb;
            double eadt = exp(a * dt);
            Tb1 = (b * (-a * dt + eadt - 1) + a * (c * (eadt - 1) + a * Tb0 * eadt)) / a2;
        }
        return Tb1;
    }
}

namespace Sgt
{
    void SimpleBuilding::initializeState()
    {
        Tb_ = TbInit_;
        setOperatingParams(lastUpdated());
    }

    void SimpleBuilding::updateState(Time t)
    {
        double dt = lastUpdated() == posix_time::neg_infin ? 0 : dSeconds(t - lastUpdated());

        if (!isMaxed_)
        {
            // Normal operation with controllers.
            Tb_ = propTbNorm(dt, Tb_, Ts_, kb_, kh_, Cb_);
        }
        else
        {
            // Power is constrained by maximum.
            Tb_ = propTbMaxed(dt, Tb_, dQg_->value(lastUpdated()), dQg_->value(t),
                    weather_->data.temperature(lastUpdated()), weather_->data.temperature(t), kb_, dQh_, Cb_);
        }
        setOperatingParams(t);
    }

    void SimpleBuilding::setOperatingParams(Time t)
    {
        dQh_ = -dQg_->value(t) + kb_ * (Ts_ - weather_->data.temperature(t)) + kh_ * (Ts_ - Tb_); // Heat ADDED.
        mode_ = dQh_ > 0 ? HvacMode::HEATING : HvacMode::COOLING;
        cop_ = mode_ == HvacMode::HEATING ? copHeat_ : copCool_;
        Ph_ = std::abs(dQh_) / cop_;
        if (Ph_ > PMax_)
        {
            Ph_ = PMax_;
            dQh_ = mode_ == HvacMode::HEATING ? Ph_ * cop_ : -Ph_ * cop_;
            isMaxed_ = true;
        }
        else
        {
            isMaxed_ = false;
        }
    }
}
