#ifndef SUN_DOT_H
#define SUN_DOT_H

#include "Common.h"

namespace SmartGridToolbox
{
   struct SphericalAnglesRadians
   {
      double zenith;    ///< Angle between directly overhead and the desired point on sphere.
      double azimuth;   ///< Direction of point, angle clockwise from due north(?).
   };

   inline Array<double, 3> vecFromSpherical(const SphericalAnglesRadians & angs, double mag)
   {
      using std::cos;
      using std::sin;
      return {mag * cos(angles.azimuth) * sin(angles.zenith), mag * sin(angles.azimuth) * sin(angles.zenith),
              mag * cos(angles.zenith)};
   }

   /// Zenith and azimuth angles of the sun at a given time and location.
   /** @parameter utcTime: UTC time.
    *  @parameter planeNormal: The coordinates specified by the normal of a plane. 
    *  @return Struct containing zenith and azimuth angles of the sun. */
   SphericalAnglesRadians sunPos(ptime utcTime, LatLong location);

   /// Solar irradiance, W/m^2.
   /** @parameter sunCoords : Struct containing zenith and azimuth angles of the sun.
    *  @return Irradiance vector in W/m^2. Direction of vector points to the sun. */
   Array<double, 3> sunIrradianceVec(SphericalAnglesRadians sunCoords);

   /// Solar power falling on a plane, W.
   /** @parameter sunCoords : Struct containing zenith and azimuth angles of the sun.
    *  @parameter planeNormal: The coordinates specified by the normal of a plane. 
    *  @parameter planeArea: The area of the plane in m^2.
    *  @return Power in W. Angle between sun and plane normal of >= 90 degrees implies zero power. */
   double sunPower(SphericalAnglesRadians sunCoords, Array<double, 3> plane);

   /// Solar power per m^2 falling on a plane, W/m^2.
   /** @parameter sunCoords : Struct containing zenith and azimuth angles of the sun.
    *  @parameter planeNormal: The coordinates specified by the normal of a plane. 
    *  @return Irradiance in W/m^2. Angle between sun and plane normal of >= 90 degrees implies zero power. */
   double sunIrradiance(SphericalAnglesRadians sunCoords, SphericalAnglesRadians planeNormal)
   {
      return sunPower(sunCoords, vecFromSpherical(planeNormal, 1.0));
   }
}

#endif // SUN_DOT_H
