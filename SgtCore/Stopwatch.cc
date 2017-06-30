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

#include "Stopwatch.h"

namespace Sgt
{

    void Stopwatch::start()
    {
        isRunning_ = true;
        wallStart_ = std::chrono::system_clock::now();
        cpuStart_ = std::clock();
    }

    void Stopwatch::stop()
    {
        if (isRunning_)
        {
            wallDur_ += std::chrono::system_clock::now() - wallStart_;
            cpuDur_ += static_cast<double>(std::clock() - cpuStart_) / CLOCKS_PER_SEC;
        }
        isRunning_ = false;
    }

    void Stopwatch::reset()
    {
        wallDur_ = std::chrono::duration<double>::zero();
        cpuDur_ = 0;
        if (isRunning_)
        {
            wallStart_ = std::chrono::system_clock::now();
            cpuStart_ = std::clock();
        }

    }

    double Stopwatch::wallSeconds() const
    {
        return isRunning_
            ? (wallDur_ + std::chrono::duration<double>(std::chrono::system_clock::now() - wallStart_)).count()
            : wallDur_.count();
    }
    
    double Stopwatch::cpuSeconds() const
    {
        return isRunning_
            ? (cpuDur_ + static_cast<double>(std::clock() - cpuStart_) / CLOCKS_PER_SEC)
            : cpuDur_;
    }
}
