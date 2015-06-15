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

#ifndef SUN_DOT_H
#define SUN_DOT_H

#include <SgtCore/Common.h>

namespace Sgt
{
    constexpr double solarIrradianceMag() {return 1367;} // W/m^2, atmospheric absorbance not taken into account.

    struct SphericalAngles
    {
        double zenith;    ///< Angle in radians between directly overhead and the desired point on sphere.
        double azimuth;   ///< Direction of point in radians, angle clockwise from due north(?).
    };

    /// @brief Convert spherical angles and magnitude to a vector.
    ///
    /// x -> north, y -> east, z -> directly up.
    /// @param angs Struct containing spherical angles of the sun.
    /// @param mag Magnitude of the vector.
    /// @return The vector.
    inline Array<double, 3> angsAndMagToVec(const SphericalAngles& angs, double mag)
    {
        using std::cos;
        using std::sin;
        return {{
                mag * cos(angs.azimuth) * sin(angs.zenith), mag * sin(angs.azimuth) * sin(angs.zenith),
                mag * cos(angs.zenith)
            }};
    }

    /// @brief Convert spherical angles and projection to a vector.
    /// @param angs struct containing spherical angles of the sun.
    /// @param proj projection of the vector onto a vertical vector.
    /// @return The vector.
    ///
    /// x -> north, y -> east, z -> directly up.
    /// If the vector is interpreted as the normal to an area, then the projection is interpreted as being
    /// onto the horizontal plane.
    inline Array<double, 3> angsAndProjToVec(const SphericalAngles& angs, double proj)
    {
        using std::cos;
        using std::sin;

        return angsAndMagToVec(angs, proj / cos(angs.zenith));
    }

    /// @brief Spherical angles of the sun at a given time and location.
    /// @param utcTime UTC time.
    /// @param location Lat long of the location.
    /// @return Spherical angles of the sun.
    SphericalAngles sunPos(posix_time::ptime utcTime, LatLong location);

    /// @brief Horizontal solar irradiance.
    ///
    /// This quantity is often used as a proxy for the solar irradiance magnitude, as it is easier to measure.
    /// @param angs Spherical angles of the sun.
    /// @return Horizontal solar irradiance of the sun, in W/m^2.
    inline double horizontalSolarIrradiance(const SphericalAngles& angs)
    {
        double cosZen = cos(angs.zenith);
        return (cosZen > 0.0 ? solarIrradianceMag() * cosZen : 0.0);
    }

    /// @brief Solar irradiance vector, W/m^2.
    /// @param solarAngles Solar angles of the sun.
    /// @return Irradiance vector in W/m^2. Direction of vector points to the sun.
    inline Array<double, 3> solarIrradianceVec(SphericalAngles solarAngles)
    {
        return angsAndMagToVec(solarAngles, solarIrradianceMag());
    }

    /// @brief Solar power falling on a plane, W.
    /// @param solarAngles Spherical angles of the sun.
    /// @param planeNormal The coordinates specified by the normal of a plane.
    /// @param planeArea The area of the plane.
    /// @return Power in W. Angle between sun and plane normal of >= 90 degrees implies zero power.
    double solarPower(SphericalAngles solarAngles, SphericalAngles planeNormal, double planeArea);

    /// @brief Solar power per m^2 falling on a plane, W/m^2.
    /// @param solarAngles Struct containing zenith and azimuth angles of the sun.
    /// @param planeNormal The coordinates specified by the normal of a plane.
    /// @return Irradiance in W/m^2. Angle between sun and plane normal of >= 90 degrees implies zero power.
    inline double solarIrradiance(SphericalAngles solarAngles, SphericalAngles planeNormal)
    {
        return solarPower(solarAngles, planeNormal, 1.0);
    }
}

#endif // SUN_DOT_H
