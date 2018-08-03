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

#include "RealTimeClock.h"

#include <thread>

using namespace std;

namespace Sgt
{
    void RealTimeClock::fastForward(const Time& until, bool block)
    {
        sgtLogDebug() << "RealTimeClock: fastForward(...): " << localTimeString(until) << endl;
        sgtLogIndent();
        unique_lock<mutex> lk(mut_);
        sw_.stop();
        fastForwarding_ = true;
        stopUntil_ = until;
        if (block)
        {
            cv_.wait(lk, [this]{return fastForwarding_ == false;});
        }
        sgtLogDebug() << "RealTimeClock: fastForward(...): Finished. lastUpdated = "
            << localTimeString(lastUpdated()) << endl;
    }

    void RealTimeClock::initializeState()
    {
        Heartbeat::initializeState();
    }

    void RealTimeClock::updateState(const Time& t)
    {
        sgtLogDebug() << "RealTimeClock: updateState(...): t = " << localTimeString(t) << endl;
        sgtLogIndent();

        long sleepForMs = -1;

        {
            unique_lock<mutex> lk(mut_);
            if (fastForwarding_)
            {
                Time nextT = t + dt();
                if (nextT >= stopUntil_)
                {
                    // We're might be a bit ahead! So need to wait a bit before update.
                    sgtLogDebug() << "RealTimeClock: updateState(...): stopUntil = "
                        << localTimeString(stopUntil_) << endl;
                    // Want to synchronize with stopUntil.
                    restartRealTime();
                    nextWallSeconds_ = (secondsPerDt_ / dSeconds(dt())) * dSeconds(nextT - stopUntil_);
                    sgtLogDebug() << "RealTimeClock: updateState:(...): nextWallSeconds_ = " << nextWallSeconds_ << endl;
                    cv_.notify_one();
                }
            }
            else 
            {
                if (!isInitialized())
                {
                    // First step, to simulation start time.
                    restartRealTime();
                }
                else
                {
                    sleepForMs = lround(1000 * (nextWallSeconds_ - sw_.wallSeconds()));
                    sgtLogDebug() << "RealTimeClock: updateState(...): sleepForMs = " << sleepForMs 
                        << ", wallSeconds = " << sw_.wallSeconds() << endl;
                    nextWallSeconds_ = nextWallSeconds_ + secondsPerDt_;
                }
            }
        }

        if (sleepForMs > 0)
        {
            this_thread::sleep_for(chrono::milliseconds(sleepForMs));
        }

        Heartbeat::updateState(t);
    }

    void RealTimeClock::restartRealTime()
    {
        sgtLogDebug() << "RealTimeClock: restartRealTime()" << endl;
        sgtLogIndent();
        sw_.stop();
        sw_.reset();
        nextWallSeconds_ = 0;
        sw_.start();
        fastForwarding_ = false;
    }
}
