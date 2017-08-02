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

#include <SgtSim/TapChanger.h>

using namespace std;
using namespace arma;

namespace Sgt
{
    TapChanger::TapChanger(
            const std::string& id,
            const ConstComponentPtr<BranchAbc, TransformerAbc>& trans,
            const std::vector<double>& taps,
            double setpoint,
            double tolerance,
            arma::uword ctrlSideIdx,
            arma::uword windingIdx,
            arma::uword ratioIdx,
            bool hasLdc,
            Complex ZLdc,
            Complex topFactorI) :
        Component(id),
        trans_(trans),
        taps_(taps),
        setpoint_(setpoint),
        tolerance_(tolerance),
        ctrlSideIdx_(ctrlSideIdx),
        windingIdx_(windingIdx),
        ratioIdx_(ratioIdx),
        hasLdc_(hasLdc),
        ZLdc_(ZLdc),
        topFactorI_(topFactorI)
    {
        needsUpdate().addTrigger(trans_->bus1()->voltageUpdated());
        // Slightly KLUDGey: If the bus voltage updates, then so might the winding voltage.
    }

    void TapChanger::initializeState()
    {
        prevTimestep_ = posix_time::neg_infin;
    }

    // TODO: what if the deadband is too small and we're alternating settings?
    void TapChanger::updateState(Time t)
    {
        sgtLogDebug() << sComponentType() << " " << id() << " : Update : " << prevTimestep_ << " -> " << t << std::endl;
        sgtLogIndent();
        bool tryAgain = false;

        bool isFirstStep = (prevTimestep_ == posix_time::neg_infin);
        if (isFirstStep)
        {
            sgtLogDebug() << sComponentType() << " " << id() << " : First iteration" << std::endl;
            // Must be my first update in the simulation. Do a special iteration.
            setting_ = (taps_.size() - 1) / 2;
            tryAgain = true;
        }
        else
        {
            sgtLogDebug() << sComponentType() << " " << id() << " : Not first iteration" << std::endl;
            bool isNewTimestep = (t != prevTimestep_);
            if (isNewTimestep)
            {
                sgtLogDebug() << sComponentType() << " " << id() << " : New timestep" << std::endl;
                iter_ = 0;
            }
            else
            {
                sgtLogDebug() << sComponentType() << " " << id() << " : Repeated timestep : " << iter_ << std::endl;
                ++iter_;
            }

            if (iter_ < taps_.size())
            {
                sgtLogDebug() << sComponentType() << " " << id() << " : Try update" << std::endl;
                val_ = get();
                sgtLogDebug() << sComponentType() << " " << id() << " : Val = " << val_ << std::endl;
                double delta = val_ - setpoint_;
                sgtLogDebug() << sComponentType() << " " << id() << " : Delta = " << delta << std::endl;
                if (abs(delta) >= tolerance_)
                {
                    sgtLogDebug() << sComponentType() << " " << id() << " : Out of tolerance" << std::endl;
                    if (delta > 0 && setting_ != 0)
                    {
                        sgtLogDebug() << sComponentType() << " " << id() << " : Reduce setting" << std::endl;
                        // Above the setpoint and can reduce setting.
                        --setting_;
                        tryAgain = true;
                    }
                    else if (delta < 0 && setting_ != taps_.size() - 1)
                    {
                        sgtLogDebug() << sComponentType() << " " << id() << " : Increase setting" << std::endl;
                        // Below the setpoint and can increase setting.
                        ++setting_;
                        tryAgain = true;
                    }
                    else
                    {
                        sgtLogDebug() << sComponentType() << " " << id() << " : No more settings" << std::endl;
                    }
                }
                else
                {
                    sgtLogDebug() << sComponentType() << " " << id() << " : Within tolerance" << std::endl;
                }
            }
        }
        sgtLogDebug() << sComponentType() << " " << id() << " : Setting = " << setting_ << std::endl;
        if (tryAgain)
        {
            sgtLogDebug() << sComponentType() << " " << id() << " : Setting was updated; try again" << std::endl;
            set(taps_[setting_]);
        }
        prevTimestep_ = t;
        ++iter_;
    }

    double TapChanger::get() const
    {
        Complex V = (ctrlSideIdx_ == 0 ? trans_->VWindings0() : trans_->VWindings1())[windingIdx_];
        if (hasLdc_)
        {
            Complex I = (ctrlSideIdx_ == 0 ? trans_->IWindings0() : trans_->VWindings1())[windingIdx_] * topFactorI_;
            V = V - I * ZLdc_;
        }
        return abs(V);
    }
    
    void TapChanger::set(double val)
    {
        trans_->setOffNomRatio(val, ratioIdx_);
    }
}
