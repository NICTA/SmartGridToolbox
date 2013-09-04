#ifndef SUN_DOT_H
#define SUN_DOT_H

#include "Common.h"

namespace SmartGridToolbox
{
   struct SunCoordsRadians
   {
      double zenith;    ///< Angle between directly overhead and the center of sun's disk.
      double azimuth;   ///< Direction of sun, angle clockwise from due north(?).
   };

   /// Zenith and azimuth angles of the sun at a given time and location.
   /** @parameter utcTime: UTC time.
    *  @parameter planeNormal: The coordinates specified by the normal of a plane. 
    *  @return Struct containing zenith and azimuth angles of the sun. */
   SunCoordsRadians sunPos(ptime utcTime, LatLong location);

   /// Solar irradiance, W/m^2.
   /** @parameter sunCoords : Struct containing zenith and azimuth angles of the sun.
    *  @return Irradiance vector in W/m^2. Direction of vector points to the sun. */
   Array<double, 3> sunIrradianceVec(SunCoordsRadians sunCoords);

   /// Solar power falling on a plane, W.
   /** @parameter sunCoords : Struct containing zenith and azimuth angles of the sun.
    *  @parameter planeNormal: The coordinates specified by the normal of a plane. 
    *  @parameter planeArea: The area of the plane in m^2.
    *  @return Power in W. Angle between sun and plane normal of >= 90 degrees implies zero power. */
   double sunPower(SunCoordsRadians sunCoords, SunCoordsRadians planeNormal, double planeArea);

   /// Solar power per m^2 falling on a plane, W/m^2.
   /** @parameter sunCoords : Struct containing zenith and azimuth angles of the sun.
    *  @parameter planeNormal: The coordinates specified by the normal of a plane. 
    *  @return Irradiance in W/m^2. Angle between sun and plane normal of >= 90 degrees implies zero power. */
   double sunIrradiance(SunCoordsRadians sunCoords, SunCoordsRadians planeNormal)
   {
      return sunPower(sunCoords, planeNormal, 1.0);
   }
}

#endif // SUN_DOT_H
