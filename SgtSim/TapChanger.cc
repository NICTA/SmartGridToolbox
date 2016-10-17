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

namespace Sgt
{
    void TapChanger::initializeState()
    {
        idx_ = (taps_.size() - 1) / 2; // Start in the middle of the range.
        isFirstStep_ = false;
        isDone_ = false;
    }

    // TODO: what if the deadband is too small and we're alternating settings?
    void TapChanger::updateState(Time t)
    {
        isDone_ = false;

        if (isFirstStep_)
        {
            set_(taps_[idx_]);
            isFirstStep_ = false;
            return;
        }

        val_ = get_();
        double delta = val_ - setpoint_;

        if (std::abs(delta / setpoint_) <= tolerance_)
        {
            // We're finished.
            isDone_ = true;
        }
        else
        {
            // Try again.
            if (delta > 0)
            {
                // We're above the setpoint, reduce the setting.
                if (idx_ == 0)
                {
                    isDone_ = true;
                }
                else
                {
                    --idx_;
                }
            }
            else
            {
                // We're below the setpoint, reduce the setting.
                if (idx_ == taps_.size() - 1)
                {
                    isDone_ = true;
                }
                else
                {
                    ++idx_;
                }
            }
            if (!isDone_)
            {
                set_(taps_[idx_]); // Update the setting.
            }
        }
    }
}
