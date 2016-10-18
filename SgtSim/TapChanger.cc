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
    void TapChanger::initializeState()
    {
        prevTimestep_ = posix_time::neg_infin;
    }

    // TODO: what if the deadband is too small and we're alternating settings?
    void TapChanger::updateState(Time t)
    {
        sgtLogDebug() << sComponentType() << " : Update : " << prevTimestep_ << " -> " << t << std::endl;
        LogIndent _;
        bool tryAgain = false;

        bool isFirstStep = (prevTimestep_ == posix_time::neg_infin);
        if (isFirstStep)
        {
            sgtLogDebug() << sComponentType() << " : First iteration" << std::endl;
            // Must be my first update in the simulation. Do a special iteration.
            setting_ = (taps_.size() - 1) / 2;
            tryAgain = true;
        }
        else
        {
            sgtLogDebug() << sComponentType() << " : Not first iteration" << std::endl;
            bool isNewTimestep = (t != prevTimestep_);
            if (isNewTimestep)
            {
                sgtLogDebug() << sComponentType() << " : New timestep" << std::endl;
                iter_ = 0;
            }
            else
            {
                sgtLogDebug() << sComponentType() << " : Repeated timestep : " << iter_ << std::endl;
                ++iter_;
            }

            if (iter_ < taps_.size())
            {
                LogIndent _;
                sgtLogDebug() << sComponentType() << " : Try update" << std::endl;
                val_ = get_();
                sgtLogDebug() << sComponentType() << " : Val = " << val_ << std::endl;
                double delta = val_ - setpoint_;
                sgtLogDebug() << sComponentType() << " : Delta = " << delta << std::endl;
                if (abs(delta / setpoint_) >= tolerance_)
                {
                    sgtLogDebug() << sComponentType() << " : Out of tolerance" << std::endl;
                    if (delta > 0 && setting_ != 0)
                    {
                        sgtLogDebug() << sComponentType() << " : Reduce setting" << std::endl;
                        // Above the setpoint and can reduce setting.
                        --setting_;
                        tryAgain = true;
                    }
                    else if (delta < 0 && setting_ != taps_.size() - 1)
                    {
                        sgtLogDebug() << sComponentType() << " : Increase setting" << std::endl;
                        // Below the setpoint and can increase setting.
                        ++setting_;
                        tryAgain = true;
                    }
                }
            }
        }
        if (tryAgain)
        {
            sgtLogDebug() << sComponentType() << " : Set and try again" << std::endl;
            set_(taps_[setting_]);
        }
        prevTimestep_ = t;
        ++iter_;
    }
}
