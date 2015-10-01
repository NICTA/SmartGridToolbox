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

#include "TimeSeries.h"

namespace Sgt
{
    double Weather::solarPower(const Time& t, const SphericalAngles& planeNormal, double planeArea) const
    {
        // Neglect ground reflected radiation. This is reasonable, because typically a solar collector etc would
        // be pointing at a zenith angle of less than 90 degrees, so would not get a ground component.
        Array<double, 3> planeVec = angsAndMagToVec(planeNormal, planeArea);
        auto irr = irradiance(t);
        double direct = dot<double, 3>(planeVec, irr.direct);
        if (direct < 0) direct = 0;
        double diffuse = planeArea * irr.horizontalDiffuse * (pi - planeNormal.zenith) / pi;
        return direct + diffuse;
    }

    SolarIrradiance Weather::irradiance(const Time& tm) const
    {
        // This model is provisional and definitely needs to be improved.
        // Assume transmitted and diffuse components vary linearly with the cloud cover coefficient.
        //
        // See http://www.powerfromthesun.net/Book/chapter02/chapter02.html for some relevant information.

        // Lots of guesses below:
        const double minDirect = 0.0;
        const double maxDirect = 0.6; // Average of clear sky values in reference above.

        const double minDiffuse = 0.025;
        const double maxDiffuse = 0.15; // Average of clear sky values in reference above.

        double cloudCover = cloudCoverSeries_->value(tm);
        assert(cloudCover >= 0 && cloudCover <= 1);

        double directFrac = maxDirect - cloudCover * (maxDirect - minDirect);
        double diffuseFrac = maxDiffuse - cloudCover * (maxDiffuse - minDiffuse);

        SphericalAngles sunAngs = sunPos(utcTime(tm), latLong_);

        SolarIrradiance result;

        if (sunAngs.zenith < pi / 2)
        {
            result.direct = directFrac * solarIrradianceVec(sunAngs);
            result.horizontalDiffuse = diffuseFrac * solarIrradianceMag();
        }
        else
        {
            result.direct = Array<double, 3> {{0.0, 0.0, 0.0}};
            result.horizontalDiffuse = 0.0;
        }

        return result;
    }
}
