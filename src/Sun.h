#ifndef SUN_DOT_H
#define SUN_DOT_H

#include "Common.h"

namespace SmartGridToolbox
{
   constexpr double solarIrradianceInVacuo() {return 1367;} // W/m^2, atmospheric absorbance not taken into account.

   struct SphericalAnglesRadians
   {
      double zenith;    ///< Angle between directly overhead and the desired point on sphere.
      double azimuth;   ///< Direction of point, angle clockwise from due north(?).
   };

   inline Array<double, 3> vecFromSpherical(const SphericalAnglesRadians & angs, double mag)
   {
      using std::cos;
      using std::sin;
      return {mag * cos(angs.azimuth) * sin(angs.zenith), mag * sin(angs.azimuth) * sin(angs.zenith),
              mag * cos(angs.zenith)};
   }

   /// Zenith and azimuth angles of the sun at a given time and location.
   /** @parameter utcTime: UTC time.
    *  @parameter planeNormal: The coordinates specified by the normal of a plane. 
    *  @return Struct containing zenith and azimuth angles of the sun. */
   SphericalAnglesRadians sunPos(ptime utcTime, LatLong location);

   /// Solar irradiance, W/m^2.
   /** @parameter solarAngles : Struct containing zenith and azimuth angles of the sun.
    *  @return Irradiance vector in W/m^2. Direction of vector points to the sun. */
   Array<double, 3> solarIrradianceInVacuoVec(SphericalAnglesRadians solarAngles);

   /// Solar power falling on a plane, W.
   /** @parameter solarAngles : Struct containing zenith and azimuth angles of the sun.
    *  @parameter planeNormal: The coordinates specified by the normal of a plane. 
    *  @parameter planeArea: The area of the plane in m^2.
    *  @return Power in W. Angle between sun and plane normal of >= 90 degrees implies zero power. */
   double solarPowerInVacuo(SphericalAnglesRadians solarAngles, Array<double, 3> plane);

   /// Solar power per m^2 falling on a plane, W/m^2.
   /** @parameter solarAngles : Struct containing zenith and azimuth angles of the sun.
    *  @parameter planeNormal: The coordinates specified by the normal of a plane. 
    *  @return Irradiance in W/m^2. Angle between sun and plane normal of >= 90 degrees implies zero power. */
   double solarIrradianceInVacuo(SphericalAnglesRadians solarAngles, SphericalAnglesRadians planeNormal)
   {
      return solarPowerInVacuo(solarAngles, vecFromSpherical(planeNormal, 1.0));
   }
}

#endif // SUN_DOT_H
