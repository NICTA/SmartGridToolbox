#ifndef SUN_DOT_H
#define SUN_DOT_H

#include "Common.h"

namespace SmartGridToolbox
{
   struct SunCoords
   {
      double dZenithAngle; ///< Angle between directly overhead and the center of sun's disk.
      double dAzimuth; ///< Direction of sun, angle clockwise from due north(?).
   };

   /// Attenuation factor of radiation on a plane, due to an oblique angle.
   /** @parameter sunCoords: The sun's coordinates. 
    *  @parameter planeNormal: The coordinates specified by the normal of a plane. 
    *  @return the cosine of the angle between the plane normal and the sun. */
   double angleFactor(SunCoords & sunCoords, SunCoords & planeNormal);

   /// Solar power in Watts.
   /** @parameter sunCoords: The sun's coordinates. 
    *  @parameter planeNormal: The coordinates specified by the normal of a plane. 
    *  @return the power in watts. */
   double sunPowerW(ptime utcTime, LatLong location, SunCoords planeNormal, double planeArea_m2);
}

#endif // SUN_DOT_H
