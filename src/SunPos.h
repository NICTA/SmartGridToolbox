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
   double dZenithAngle;
   double dAzimuth;
};

void sunpos(cTime udtTime, cLocation udtLocation, cSunCoordinates *udtSunCoordinates);

#endif
