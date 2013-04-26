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

         virtual V operator()(const T & t) = 0;
   };

   template<typename T, typename V>
   class DataTimeSeries : public TimeSeries<T, V>
   {
      public:
         virtual ~DataTimeSeries()
         {
            // Empty.
         }

         virtual void addPoint(const T & t, const V & v) = 0;
   };

   template<typename T, typename V>
   class StepwiseTimeSeries : public DataTimeSeries<T, V>
   {
      public:
         virtual ~StepwiseTimeSeries()
         {
            // Empty.
         }

         virtual V operator()(const T & t) override
         {
            auto pos = points_.upper_bound(dSeconds(t));
            if (pos != points_.begin())
            {
               --pos;
            }
            return pos->second;

         }

         virtual void addPoint(const T & t, const V & v) override
         {
            points_[dSeconds(t)] = v;
         }

      private:
         std::map<double, V> points_;
   };

   template<typename T, typename V>
   class LerpTimeSeries : public DataTimeSeries<T, V>
   {
      public:
         LerpTimeSeries()
         {
            // Empty.
         }

         virtual ~LerpTimeSeries()
         {
            // Empty.
         }

         virtual V operator()(const T & t) override
         {
            double td = dSeconds(t);
            auto pos2 = points_.upper_bound(td);
            if (pos2 == points_.begin())
            {
               return pos2->second;
            }
            else if (pos2 == points_.end())
            {
               return (--pos2)->second;
            }
            else
            {
               auto pos1 = pos2; --pos1;
               return pos1->second + 
                  (pos2->second - pos1->second) * (td - pos1->first) / 
                  (pos2->first - pos1->first);
            }
         }

         virtual void addPoint(const T & t, const V & v) override
         {
            points_[dSeconds(t)] = v;
         }

      private:
         std::map<double, V> points_;
   };

   template<typename T>
   class SplineTimeSeries : public DataTimeSeries<T, double>
   {
      public:
         virtual ~SplineTimeSeries()
         {
            // Empty.
         }

         virtual double operator()(const T & t) override
         {
            return spline_(dSeconds(t));
         }

         virtual void addPoint(const T & t, const double & v) override
         {
            spline_.addPoint(dSeconds(t), v);
         }

      private:

         Spline spline_;
   };
}

#endif // TIME_SERIES_DOT_H
