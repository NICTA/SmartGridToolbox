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

#ifndef SOLAR_GEOM_DOT_H
#define SOLAR_GEOM_DOT_H

#include <SgtCore/Common.h>

namespace Sgt
{
    struct SphericalAngles
    {
        double zenith;    ///< Angle in radians between directly overhead and the desired point on sphere.
        double azimuth;   ///< Direction of point in radians, angle clockwise from due north(?).
    };
    
    /// @brief Convert spherical angles and magnitude to a vector.
    /// @param angs Struct containing spherical angles of the sun.
    /// @param mag Magnitude of the vector.
    /// @return The vector.
    /// x -> north, y -> east, z -> directly up.
    inline Array<double, 3> angsAndMagToVec(const SphericalAngles& angs, double mag)
    {
        return {{
            mag * std::cos(angs.azimuth) * std::sin(angs.zenith),
            mag * std::sin(angs.azimuth) * std::sin(angs.zenith),
            mag * std::cos(angs.zenith)}};
    }

    /// @brief Convert spherical angles and projection to a vector.
    /// @param angs struct containing spherical angles of the sun.
    /// @param proj projection of the vector onto a vertical vector.
    /// @return The vector.
    /// x -> north, y -> east, z -> directly up.
    /// If the vector is interpreted as the normal to an area, then the projection is interpreted as being
    /// onto the horizontal plane.
    inline Array<double, 3> angsAndProjToVec(const SphericalAngles& angs, double proj)
    {
        return angsAndMagToVec(angs, proj / std::cos(angs.zenith));
    }

    struct Irradiance
    {
        Array<double, 3> direct;
        double skyDiffuse;
        double groundDiffuse;
    };
    
    /// @brief Solar power falling on a plane, W.
    /// @param irr Irradiance struct
    /// @param planeNormal The coordinates specified by the normal of a plane.
    /// @param planeArea The area of the plane.
    /// @return Power in W. Angle between sun and plane normal of >= 90 degrees implies zero power.
    double solarPower(const Irradiance& irradiance, const SphericalAngles& planeNormal, double planeArea);

    /// @brief Solar power per m^2 falling on a plane, W/m^2.
    /// @param irr Irradiance struct
    /// @param planeNormal The coordinates specified by the normal of a plane.
    inline double solarIrradiance(const Irradiance& irr, const SphericalAngles& planeNormal)
    {
        return solarPower(irr, planeNormal, 1.0);
    }
}

#endif // SOLAR_GEOM_DOT_H
