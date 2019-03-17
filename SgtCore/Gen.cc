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

#include "Gen.h"

#include <ostream>

namespace Sgt
{
    json Gen::toJson() const
    {
        json j = Component::toJson();
        j[sComponentType()] = {
            {"phases: ", phases()},
            {"S: ", S()},
            {"PMin: ", PMin()},
            {"PMax: ", PMax()},
            {"QMin: ", QMin()},
            {"QMax: ", QMax()},
            {"cStartup : ", cStartup()},
            {"cShutdown: ", cShutdown()},
            {"c0: ", c0()},
            {"c1: ", c1()},
            {"c2: ", c2()}};
        return j;
    }
            
    void Gen::setIsInService(bool isInService)
    {
        if (isInService != isInService_)
        {
            isInService_ = isInService;
            isInServiceChanged_.trigger();
        }
    }

    double Gen::cost() const
    {
        double P = 0;
        for (const auto& Pi : S())
        {
            P += Pi.real();
        }
        return c0() + c1() * P + c2() * P * P;
    }
}
