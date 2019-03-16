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

#include "SolarGeom.h"

namespace Sgt
{
    double solarPower(const Irradiance& irradiance, const SphericalAngles& planeNormal, double planeArea)
    {
        arma::Col<double>::fixed<3> planeVec = angsAndMagToVec(planeNormal, planeArea);
        double direct = arma::dot(planeVec, irradiance.direct);
        if (direct < 0) direct = 0;
        double cd2 = 0.5 * std::cos(planeNormal.zenith);
        double diffuse = planeArea * (irradiance.skyDiffuse * (0.5 + cd2) + irradiance.groundDiffuse * (0.5 - cd2));
        return direct + diffuse;
    }
}
