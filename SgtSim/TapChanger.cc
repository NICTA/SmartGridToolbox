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
    TapChangerAbc::TapChangerAbc(
            const ConstComponentPtr<BranchAbc, TransformerAbc>& trans,
            arma::uword ratioIdx,
            double minTapRatio,
            double maxTapRatio,
            int minTap,
            std::size_t nTaps) :
        trans_(trans),
        ratioIdx_(ratioIdx),
        tapRatios_(nTaps)
    {
        double dTap = (maxTapRatio - minTapRatio) / (nTaps - 1);
        for (std::size_t i = 0; i < nTaps; ++i) tapRatios_[i] = maxTapRatio - i * dTap; // Reverse order.
        // Slightly KLUDGey: If the bus voltage updates, then so might the winding voltage.
    }

    AutoTapChanger::AutoTapChanger(
            const std::string& id,
            const ConstComponentPtr<BranchAbc, TransformerAbc>& trans,
            arma::uword ratioIdx,
            double minTapRatio,
            double maxTapRatio,
            int minTap,
            std::size_t nTaps,
            double setpoint,
            double tolerance,
            arma::uword ctrlSideIdx,
            arma::uword ctrlWindingIdx,
            bool hasLdc,
            Complex ZLdc,
            Complex ldcTopFactorI) :
        Component(id),
        TapChangerAbc(trans, ratioIdx, minTapRatio, maxTapRatio, minTap, nTaps),
        setpoint_(setpoint),
        tolerance_(tolerance),
        ctrlSideIdx_(ctrlSideIdx),
        ctrlWindingIdx_(ctrlWindingIdx),
        hasLdc_(hasLdc),
        ZLdc_(ZLdc),
        ldcTopFactorI_(ldcTopFactorI)
    {
        needsUpdate().addTrigger(trans_->bus1()->voltageUpdated());
    }

    void AutoTapChanger::initializeState()
    {
        prevTimestep_ = TimeSpecialValues::neg_infin;
    }

    // TODO: what if the deadband is too small and we're alternating taps?
    void AutoTapChanger::updateState(const Time& t)
    {
        sgtLogDebug() << sComponentType() << " " << id() << " : Update : " << prevTimestep_ << " -> " << t << std::endl;
        sgtLogIndent();
        bool tryAgain = false;

        bool isFirstStep = (prevTimestep_ == TimeSpecialValues::neg_infin);
        if (isFirstStep)
        {
            sgtLogDebug() << sComponentType() << " " << id() << " : First iteration" << std::endl;
            // Must be my first update in the simulation. Do a special iteration.
            tap_ = minTap_ + static_cast<int>((tapRatios_.size() - 1) / 2);
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
            }

            // if (iter_ < tapRatios_.size())
            {
                sgtLogDebug() << sComponentType() << " " << id() << " : Try update" << std::endl;
                ctrlV_ = getCtrlV();
                sgtLogDebug() << sComponentType() << " " << id() << " : CtrlV = " << ctrlV_ << std::endl;
                double delta = ctrlV_ - setpoint_;
                sgtLogDebug() << sComponentType() << " " << id() << " : Delta = " << delta << std::endl;
                if (abs(delta) >= tolerance_)
                {
                    sgtLogDebug() << sComponentType() << " " << id() << " : Out of tolerance" << std::endl;
                    if (delta > 0 && tapIdx() != 0)
                    {
                        sgtLogDebug() << sComponentType() << " " << id() << " : Reduce tap" << std::endl;
                        // Above the setpoint and can reduce tap.
                        setTap(tap_ - 1);
                        tryAgain = true;
                    }
                    else if (delta < 0 && tapIdx() != tapRatios_.size() - 1)
                    {
                        sgtLogDebug() << sComponentType() << " " << id() << " : Increase tap" << std::endl;
                        // Below the setpoint and can increase tap.
                        setTap(tap_ + 1);
                        tryAgain = true;
                    }
                    else
                    {
                        sgtLogDebug() << sComponentType() << " " << id() << " : No more taps" << std::endl;
                    }
                }
                else
                {
                    sgtLogDebug() << sComponentType() << " " << id() << " : Within tolerance" << std::endl;
                }
            }
        }
        sgtLogDebug() << sComponentType() << " " << id() << " : Tap = " << tap_ << std::endl;
        if (tryAgain)
        {
            sgtLogDebug() << sComponentType() << " " << id() << " : Tap was updated; try again" << std::endl;
        }
        prevTimestep_ = t;
        ++iter_;
    }

    double AutoTapChanger::getCtrlV() const
    {
        Complex V = (ctrlSideIdx_ == 0 ? trans_->VWindings0() : trans_->VWindings1())[ctrlWindingIdx_];
        if (hasLdc_)
        {
            Complex I = (ctrlSideIdx_ == 0 ? trans_->IWindings0() : trans_->IWindings1())[ctrlWindingIdx_] * ldcTopFactorI_;
            V = V - I * ZLdc_;
        }
        return abs(V);
    }


    void TimeSeriesTapChanger::updateState(const Time& t)
    {
        Heartbeat::updateState(t);
        setTap(static_cast<int>(std::lround(series_->value(t))));
    }
}
