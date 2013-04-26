#ifndef TIME_SERIES_DOT_H
#define TIME_SERIES_DOT_H

#include "Spline.h"

namespace SmartGridToolbox
{
   template<typename T, typename V>
   class TimeSeries
   {
      public:
         virtual ~TimeSeries()
         {
            // Empty.
         }

         V operator()(const T & t)
         {
            return getVal(dseconds(t));
         }

      private:
         virtual V getVal(double s) = 0;
   };

   template<typename T, typename V>
   class DataTimeSeries : public TimeSeries<T, V>
   {
      public:
         virtual ~DataTimeSeries()
         {
            // Empty.
         }

         void addPoint(const T & t, const V & v)
         {
            addPt(dseconds(t), v);
         }

      private:
         virtual void addPt(double s, const V & v) = 0;
   };
   
   template<typename T>
   class SplineTimeSeries : public DataTimeSeries<T, double>
   {
      public:
         virtual ~SplineTimeSeries()
         {
            // Empty.
         }

      private:
         virtual double getVal(double s) override
         {
            return spline_(s);
         }

         virtual void addPt(double s, const double & v)
         {
            spline_.addPoint(s, v);
         }

      private:

         Spline spline_;
   };
}

#endif // TIME_SERIES_DOT_H
