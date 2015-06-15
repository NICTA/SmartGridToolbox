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

// Much of this file is based on subpos.h and sunpos.cpp
// http://www.psa.es/sdg/sunpos.htm

#include "Sun.h"

namespace Sgt
{
    namespace
    {
        // Declaration of some constants
        const double twopi = 2 * pi;
        const double rad = pi / 180;
        const double dEarthMeanRadius = 6371.01; // km.
        const double dAstronomicalUnit = 149597890.0; // km.
    }

    SphericalAngles sunPos(posix_time::ptime utcTime, LatLong location)
    {
        // Note: in original code, time was "udtTime". "UT" was also mentioned. Not sure exactly what "UDT" refers to
        // but UT is probably either UT1 or UTC, both being approximately equivalent. So I changed variable name
        // to "utcTime".

        // Main variables
        SphericalAngles result;

        double dElapsedJulianDays;
        double dHours;
        double dEclipticLongitude;
        double dEclipticObliquity;
        double dRightAscension;
        double dDeclination;

        // Auxiliary variables
        double dY;
        double dX;

        // Calculate difference in days between the current Julian Day and JD 2451545.0, which is noon 1 January 2000
        // Universal Time
        {
            // TODO: can use boost date::julian_day, but we'd need to check that this returns the same result.
            // Defer this until testing can be done.

            // Calculate time of the day in UT decimal hours
            dHours = dSeconds(utcTime.time_of_day()) / 3600.0;

            // Calculate current Julian Day
            long int liAux1 = (utcTime.date().month() - 14) / 12;
            long int liAux2 = (1461 * (utcTime.date().year() + 4800 + liAux1)) / 4
                              + (367 * (utcTime.date().month() - 2 - 12 * liAux1)) / 12
                              - (3 * ((utcTime.date().year() + 4900 + liAux1) / 100)) / 4
                              + utcTime.date().day() - 32075;
            double dJulianDate = double(liAux2) - 0.5 + dHours / 24.0;

            // Calculate difference between current Julian Day and JD 2451545.0
            dElapsedJulianDays = dJulianDate - 2451545.0;
        }

        // Calculate ecliptic coordinates (ecliptic longitude and obliquity of the ecliptic in radians but without
        // limiting the angle to be less than 2 * Pi (i.e., the result may be greater than 2 * Pi)
        {
            double dOmega = 2.1429 - 0.0010394594 * dElapsedJulianDays;
            double dMeanLongitude = 4.8950630 + 0.017202791698 * dElapsedJulianDays; // Radians
            double dMeanAnomaly = 6.2400600 + 0.0172019699 * dElapsedJulianDays;
            dEclipticLongitude = dMeanLongitude + 0.03341607 * sin(dMeanAnomaly)
                                 + 0.00034894 * sin(2 * dMeanAnomaly) - 0.0001134 - 0.0000203 * sin(dOmega);
            dEclipticObliquity = 0.4090928 - 6.2140e-9 * dElapsedJulianDays
                                 + 0.0000396 * cos(dOmega);
        }

        // Calculate celestial coordinates (right ascension and declination) in radians but without limiting the angle
        // to be less than 2 * Pi (i.e., the result may be greater than 2 * Pi)
        {
            double dSin_EclipticLongitude;
            dSin_EclipticLongitude = sin(dEclipticLongitude);
            dY = cos(dEclipticObliquity) * dSin_EclipticLongitude;
            dX = cos(dEclipticLongitude);
            dRightAscension = atan2(dY,dX);
            if(dRightAscension < 0.0) dRightAscension = dRightAscension + twopi;
            dDeclination = asin(sin(dEclipticObliquity) * dSin_EclipticLongitude);
        }

        // Calculate local coordinates (azimuth and zenith angle) in degrees
        {
            double dGreenwichMeanSiderealTime;
            double dLocalMeanSiderealTime;
            double dLatitudeInRadians;
            double dHourAngle;
            double dCos_Latitude;
            double dSin_Latitude;
            double dCos_HourAngle;
            double dParallax;
            dGreenwichMeanSiderealTime = 6.6974243242 + 0.0657098283 * dElapsedJulianDays + dHours;
            dLocalMeanSiderealTime = (dGreenwichMeanSiderealTime * 15 + location.long_) * rad;
            dHourAngle = dLocalMeanSiderealTime - dRightAscension;
            dLatitudeInRadians = location.lat_ * rad;
            dCos_Latitude = cos(dLatitudeInRadians);
            dSin_Latitude = sin(dLatitudeInRadians);
            dCos_HourAngle = cos(dHourAngle);
            result.zenith = (acos(dCos_Latitude * dCos_HourAngle * cos(dDeclination)
                                  + sin(dDeclination) * dSin_Latitude));
            dY = - sin(dHourAngle);
            dX = tan(dDeclination) * dCos_Latitude - dSin_Latitude * dCos_HourAngle;
            result.azimuth = atan2(dY, dX);
            if (result.azimuth < 0.0) result.azimuth = result.azimuth + twopi;
            result.azimuth = result.azimuth;
            // Parallax Correction
            dParallax = (dEarthMeanRadius / dAstronomicalUnit) * sin(result.zenith);
            result.zenith = (result.zenith + dParallax);
        }

        return result;
    }


    double solarPower(SphericalAngles solarAngles, SphericalAngles planeNormal, double planeArea)
    {
        Array<double, 3> xSun = solarIrradianceVec(solarAngles);
        Array<double, 3> xPlane = angsAndMagToVec(planeNormal, planeArea);
        double d = dot<double, 3>(xSun, xPlane);
        if (d < 0) d = 0;
        return d;
    }
}
