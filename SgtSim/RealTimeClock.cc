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

#include "RealTimeClock.h"

#include <thread>

using namespace std;

namespace Sgt
{
    void RealTimeClock::fastForward(const Time& until, bool blockUntilDone)
    {
        sgtLogDebug() << "RealTimeClock: fastForward(...): " << localTimeString(until) << endl;

        unique_lock<mutex> lk(mut_);
        cv_.notify_all(); // Notify thread waiting in update().

        sw_.stop();
        ffwdUntil_ = until;

        if (blockUntilDone)
        {
            sgtLogDebug() << "RealTimeClock: fastForward(...): Blocking calling thread." << endl;
            cv_.wait(lk, [this]{return isFastForwarding() == false;});
            sgtLogDebug() << "RealTimeClock: fastForward(...): Done blocking calling thread." << endl;
        }
        sgtLogDebug() << "RealTimeClock: fastForward(...): Finished. lastUpdated = "
            << localTimeString(lastUpdated()) << endl;
    }
        
    Time RealTimeClock::validUntil() const
    {
        Time hbValidUntil = Heartbeat::validUntil();
        return isFastForwarding() ? min(hbValidUntil, ffwdUntil_) : hbValidUntil;
    }

    void RealTimeClock::initializeState()
    {
        Heartbeat::initializeState();
    }

    void RealTimeClock::updateState(const Time& t)
    {
        sgtLogDebug() << "RealTimeClock: updateState(...): "
            << localTimeString(lastUpdated()) << " -> " << localTimeString(t) << endl;

        if (!isFastForwarding())
        {
            // Real time step.
            if (!isInitialized())
            {
                // First step, to simulation start time.
                sgtLogDebug() << "RealTimeClock: Initialization step." << endl; 
                restartRealTime();
            }
            else
            {
                sgtLogDebug() << "RealTimeClock: Normal step." << endl; 
                unique_lock<mutex> lk(mut_);
                double dtSimSeconds = dSeconds(t - lastUpdated());
                double dtRealSeconds = dtSimSeconds * realSecondsPerSimSecond_;
                long sleepForMs = max(lround(1000 * (dtRealSeconds - sw_.wallSeconds())), 0L);
                sgtLogDebug() << "RealTimeClock: updateState(...): wallSeconds      = " << sw_.wallSeconds() << endl; 
                sgtLogDebug() << "RealTimeClock: updateState(...): sleepForMs       = " << sleepForMs << endl; 
                cv_.wait_for(lk, chrono::milliseconds(sleepForMs), [this](){return isFastForwarding() == true;});
            }
            sw_.reset();
        }
        else
        {
            sgtLogDebug() << "RealTimeClock: Fast forward step." << endl; 
            if (t == ffwdUntil_)
            {
                restartRealTime();
            }
        }
    
        Heartbeat::updateState(t);
    }

    void RealTimeClock::restartRealTime()
    {
        sgtLogDebug() << "RealTimeClock: restartRealTime()" << endl;
        unique_lock<mutex> lk(mut_);
        sw_.stop();
        sw_.reset();
        ffwdUntil_ = TimeSpecialValues::not_a_date_time;
        sw_.start();
        cv_.notify_all();
    }
}
