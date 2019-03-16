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

// Much of this file is based on subpos.h and sunpos.cpp
// http://www.psa.es/sdg/sunpos.htm

#ifndef SUN_DOT_H
#define SUN_DOT_H

#include <SgtSim/SolarGeom.h>

#include <SgtCore/Common.h>

namespace Sgt
{
    /// @brief Spherical angles of the sun at a given time and location.
    /// @param utcTime UTC time.
    /// @param location Lat long of the location.
    /// @return Spherical angles of the sun.
    SphericalAngles sunPos(const Time& time, const LatLong& location);

    /// @brief Magnitude of clear sky direct irradiance, given position of sun and optional altitude.
    /// @param angs position of sun expressed as SphericalAngles.
    /// @param altitudeMeters Altitude in meters, default = 0.
    /// @return Magnitude of direct irradiance vector.
    /// See http://www.pveducation.org/pvcdrom/properties-of-sunlight/air-mass
    double directIrradianceMag(const SphericalAngles& angs, double altitudeMeters = 0.0);

    /// @brief Horizontal clear sky diffuse irradiance, given direct irradiance.
    /// @param directIrradianceMag Magnitude of direct irradiance, W/m^2.
    /// @return Horizontal diffuse irradiance.
    /// Just use 10% of direct magnitude, See http://www.pveducation.org/pvcdrom/properties-of-sunlight/air-mass.
    inline double skyDiffuseIrradiance(double directIrradianceMag)
    {
        // See http://www.pveducation.org/pvcdrom/properties-of-sunlight/air-mass
        return 0.1 * directIrradianceMag;
    }

    /// @brief Ground reflected irradiance, given direct irradiance.
    /// @param directIrradianceMag Magnitude of direct irradiance, W/m^2.
    /// @return Ground reflected irradiance.
    /// Currently set to 0.0 (ignored).
    inline double groundDiffuseIrradiance(double directIrradianceMag)
    {
        return 0.0;
    }

    /// @brief Solar irradiance vector, W/m^2.
    /// @param angs Solar angles of the sun.
    /// @param altitudeMeters Altitude in meters, default = 0.
    /// @return Irradiance vector in W/m^2. Direction of vector points to the sun.
    inline arma::Col<double>::fixed<3> directIrradianceVec(const SphericalAngles& angs, double altitudeMeters = 0.0)
    {
        return angsAndMagToVec(angs, directIrradianceMag(angs, altitudeMeters));
    }

    /// @brief Solar irradiance components (direct vec, diffuse, ground), W/m^2
    /// @param angs Solar angles of the sun.
    /// @param altitudeMeters Altitude in meters, default = 0.
    /// @return Irradiance components (direct vector, diffuse, ground).
    inline Irradiance irradiance(const SphericalAngles& angs, double altitudeMeters = 0.0)
    {
        auto dir = directIrradianceMag(angs, altitudeMeters);
        auto diff = skyDiffuseIrradiance(dir);
        auto ground = groundDiffuseIrradiance(dir);
        return {angsAndMagToVec(angs, dir), diff, ground};
    }
}

#endif // SUN_DOT_H
