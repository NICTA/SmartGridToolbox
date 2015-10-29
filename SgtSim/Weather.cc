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

#include "Weather.h"

namespace Sgt
{
    Irradiance WeatherModel::sunModelIrr(const Time& t)
    {
        SphericalAngles angs = sunPos(utcTime(t), latLong);
        Irradiance irr = Sgt::irradiance(angs, altitude);
        auto atten = cloudAttenuationFactors(t, angs);
        irr.direct[0] *= atten[0]; 
        irr.direct[1] *= atten[0]; 
        irr.direct[2] *= atten[0]; 
        irr.skyDiffuse *= atten[1];
        irr.groundDiffuse *= atten[2];
        return irr;
    }
}
