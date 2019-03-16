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

#include "DcPowerSource.h"

#include <numeric>

using namespace std;

namespace Sgt
{
    const std::string& DcPowerSourceAbc::sComponentType()
    {
        static std::string result("dc_power_source");
        return result;
    }

    void DcPowerSourceAbc::setActualPDc(double actualPDc)
    {
        if (abs(actualPDc - actualPDc_) > numeric_limits<double>::epsilon())
        {
            actualPDc_ = actualPDc;
            dcPowerChanged_.trigger();
        }
    }

    json GenericDcPowerSource::toJson() const
    {
        json j = this->SimComponent::toJson();
        j[sComponentType()] = {{"DC_power", requestedPDc()}};
        return j;
    }
}
