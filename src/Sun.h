#ifndef SUN_DOT_H
#define SUN_DOT_H

#include "Common.h"

namespace SmartGridToolbox
{
   constexpr double solarIrradianceMag() {return 1367;} // W/m^2, atmospheric absorbance not taken into account.

   struct SphericalAngles
   {
      double zenith;    ///< Angle in radians between directly overhead and the desired point on sphere.
      double azimuth;   ///< Direction of point in radians, angle clockwise from due north(?).
   };

   /// Convert spherical angles and magnitude to a vector.
   /** x -> north, y -> east, z -> directly up.
    *  @parameter angs: struct containing spherical angles of the sun.
    *  @parameter mag: magnitude of the vector.
    *  @return The vector. */ 
   inline Array<double, 3> angsAndMagToVec(const SphericalAngles & angs, double mag)
   {
      using std::cos;
      using std::sin;
      return {mag * cos(angs.azimuth) * sin(angs.zenith), mag * sin(angs.azimuth) * sin(angs.zenith),
              mag * cos(angs.zenith)};
   }

   /// Convert spherical angles and projection to a vector.
   /** x -> north, y -> east, z -> directly up.
    *  @parameter angs: struct containing spherical angles of the sun.
    *  @parameter proj: projection of the vector onto a vertical vector.
    *  @return The vector.
    *  If the vector is interpreted as the normal to an area, then the projection is interpreted as being
    *  onto the horizontal plane. */
   inline Array<double, 3> angsAndProjToVec(const SphericalAngles & angs, double proj)
   {
      using std::cos;
      using std::sin;

      return angsAndMagToVec(angs, proj / cos(angs.zenith));
   }

   /// Spherical angles of the sun at a given time and location.
   /** @parameter utcTime: UTC time.
    *  @parameter planeNormal: the coordinates specified by the normal of a plane. 
    *  @return Spherical angles of the sun. */
   SphericalAngles sunPos(posix_time::ptime utcTime, LatLong location);

   /// Horizontal solar irradiance.
   /** This quantity is often used as a proxy for the solar irradiance magnitude, as it is easier to measure.
    *  @parameter angs: spherical angles of the sun.
    *  @return Horizontal solar irradiance of the sun, in W/m^2. */
   inline double horizontalSolarIrradiance(const SphericalAngles & angs)
   {
      double cosZen = cos(angs.zenith);
      return (cosZen > 0.0 ? solarIrradianceMag() * cosZen : 0.0);
   }

   /// Solar irradiance vector, W/m^2.
   /** @parameter solarAngles : solar angles of the sun.
    *  @return Irradiance vector in W/m^2. Direction of vector points to the sun. */
   inline Array<double, 3> solarIrradianceVec(SphericalAngles solarAngles)
   {
      return angsAndMagToVec(solarAngles, solarIrradianceMag());
   }

   /// Solar power falling on a plane, W.
   /** @parameter solarAngles : spherical angles of the sun.
    *  @parameter planeNormal: the coordinates specified by the normal of a plane. 
    *  @parameter planeArea: the area of the plane.
    *  @return Power in W. Angle between sun and plane normal of >= 90 degrees implies zero power. */
   double solarPower(SphericalAngles solarAngles, SphericalAngles planeNormal, double planeArea);

   /// Solar power per m^2 falling on a plane, W/m^2.
   /** @parameter solarAngles : struct containing zenith and azimuth angles of the sun.
    *  @parameter planeNormal: the coordinates specified by the normal of a plane. 
    *  @return Irradiance in W/m^2. Angle between sun and plane normal of >= 90 degrees implies zero power. */
   inline double solarIrradiance(SphericalAngles solarAngles, SphericalAngles planeNormal)
   {
      return solarPower(solarAngles, planeNormal, 1.0);
   }
}

#endif // SUN_DOT_H
