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

   /// Zenith and azimuth angles of the sun at a given time and location.
   /** @parameter localTime: Local time.
    *  @parameter time_zone_ptr: The local timezone.
    *  @return Struct containing zenith and azimuth angles of the sun. */
   inline SunCoordsRadians sunPos(ptime localTime, time_zone_ptr tz, LatLong location)
   {
      return sunPos(utcTime(localTime, tz), location);
   }

   /// Attenuation factor of radiation on a plane, due to an oblique angle.
   /** @parameter sunCoords: The sun's coordinates. 
    *  @parameter planeNormal: The coordinates specified by the normal of a plane. 
    *  @return the cosine of the angle between the plane normal and the sun. */
   double angleFactor(SunCoordsRadians & sunCoords, SunCoordsRadians & planeNormal);

   /// Solar power in Watts.
   /** @parameter sunCoords: The sun's coordinates. 
    *  @parameter planeNormal: The coordinates specified by the normal of a plane. */
   double sunPowerW(SunCoordsRadians sunCoords, SunCoordsRadians planeNormal, double planeArea_m2);
}

#endif // SUN_DOT_H
