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

#ifndef STOPWATCH_DOT_H
#define STOPWATCH_DOT_H

#include <chrono>
#include <ctime>

namespace Sgt
{
    /// @brief Simple stopwatch style timer, using std::chrono.
    /// @ingroup Utilities
    class Stopwatch
    {
        public:

        void start();
        void stop();
        void reset();
        double wallSeconds() const;
        double cpuSeconds() const;
        bool isRunning() const {return isRunning_;}

        private:

        bool isRunning_{false};
        std::chrono::time_point<std::chrono::system_clock> wallStart_;
        std::chrono::duration<double> wallDur_{std::chrono::duration<double>::zero()};
        std::clock_t cpuStart_{0};
        double cpuDur_{0.0};
    };
}

#endif // STOPWATCH_DOT_H
