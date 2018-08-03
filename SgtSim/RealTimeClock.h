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

#ifndef REAL_TIME_CLOCK_DOT_H
#define REAL_TIME_CLOCK_DOT_H

#include<SgtCore/Common.h>
#include<SgtCore/Stopwatch.h>

#include<SgtSim/Heartbeat.h>

#include<chrono>
#include<condition_variable>
#include<mutex>

// Class that updates every n real seconds.
namespace Sgt
{
    class RealTimeClock : public Heartbeat
    {
        public:

        RealTimeClock(const std::string& id, const Time& dt, const Time& realTimePerDt) :
            Component(id), Heartbeat(id, dt), secondsPerDt_(dSeconds(realTimePerDt))
        {}

        /// @brief Fast forward until specified time, blocking if requested.
        void fastForward(const Time& until, bool block);

        public:

        private:

        virtual void initializeState() override;
        virtual void updateState(const Time& t) override;

        void restartRealTime();

        private:

        Stopwatch sw_;
        double nextWallSeconds_;
        double secondsPerDt_;
        bool fastForwarding_{false};
        Time stopUntil_{TimeSpecialValues::not_a_date_time};
        mutable std::mutex mut_;
        std::condition_variable cv_;
    };
}

#endif // REAL_TIME_CLOCK_DOT_H
