#include "Weather.h"

#include "TimeSeries.h"

namespace Sgt
{
    void Weather::updateState(Time t)
    {
        const double tConst = 900.0; // 15 minute smoothing constant.
        prevIrradiance_ = irradiance_;
        double prevFrac = exp(-dSeconds(t - lastUpdated()) / tConst);
        double curFrac = 1.0 - prevFrac;
        SolarIrradiance newUnav = unaveragedIrradiance(t);
        irradiance_.direct = prevFrac * prevIrradiance_.direct + curFrac * newUnav.direct;
        irradiance_.horizontalDiffuse = prevFrac * prevIrradiance_.horizontalDiffuse
                                        + curFrac * newUnav.horizontalDiffuse;
    }

    double Weather::solarPower(SphericalAngles planeNormal, double planeArea) const
    {
        // Neglect ground reflected radiation. This is reasonable, because typically a solar collector etc would
        // be pointing at a zenith angle of less than 90 degrees, so would not get a ground component.
        Array<double, 3> planeVec = angsAndMagToVec(planeNormal, planeArea);
        double direct = dot<double, 3>(planeVec, irradiance_.direct);
        if (direct < 0) direct = 0;
        double diffuse = planeArea * irradiance_.horizontalDiffuse * (pi - planeNormal.zenith) / pi;
        return direct + diffuse;
    }

    SolarIrradiance Weather::unaveragedIrradiance(const Time& tm) const
    {
        // Possibly dodgy model.
        // Assume transmitted and diffuse components vary linearly with the cloud cover coefficient.
        // Values taken haphazardly from "the web" - http://www.powerfromthesun.net/Book/chapter02/chapter02.html.

        const double maxTransmit = 0.83;
        const double minDiffuse = 0.05;

        const double minTransmit = 0.33;
        const double maxDiffuse = 0.26;

        double cloudCover = cloudCoverSeries_->value(tm);
        assert(cloudCover >= 0 && cloudCover <= 1);

        double directFrac = maxTransmit - cloudCover * (maxTransmit - minTransmit);
        double diffuseFrac = minDiffuse + cloudCover * (maxDiffuse - minDiffuse);

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
