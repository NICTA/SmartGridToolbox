// This file is available in electronic form at http://www.psa.es/sdg/sunpos.htm

#ifndef __SUNPOS_H
#define __SUNPOS_H

struct cTime
{
   int iYear;
   int iMonth;
   int iDay;
   double dHours;
   double dMinutes;
   double dSeconds;
};

struct cLocation
{
   double dLongitude;
   double dLatitude;
};

struct cSunCoordinates
{
   double dZenithAngle; ///< Angle between directly overhead and the center of sun's disk.
   double dAzimuth; ///< Direction of sun, angle clockwise from due north(?).
};

/// Attenuation factor of radiation on a plane, due to an oblique angle.
/** scPlane specifies the normal of a plane. The angleFactor is then the cos of the angle between the plane
 *  normal and the sun. */
double angleFactor(cSunCoordinates & scSun, sSunCoordinates & scPlane);

#endif
