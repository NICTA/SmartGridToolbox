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
   class LerpTimeSeries : public DataTimeSeries<T, V>
   {
      public:
         LerpTimeSeries() : isValid_(false)
         {
            // Empty.
         }

         virtual ~LerpTimeSeries()
         {
            // Empty.
         }

         virtual V operator()(const T & t) override
         {
            if (!isValid_)
            {
               validate();
            }
            double td = dSeconds(t);
            auto pos = std::find_if(points_.begin(), points_.end(),
                  [&](const std::pair<double, V> & elem)
                  {return elem.first > td;});
            if (pos == points_.end())
            {
               return (--pos)->second;
            }
            else if (pos == points_.begin()) 
            {
               return pos->second;
            }
            else
            {
               double t2 = pos->first;
               V v2 = pos->second;
               double t1 = (--pos)->first;
               V v1 = pos->second;
               return v1 + (v2 - v1) * (td - t1) / (t2 - t1);
            }
         }

         virtual void addPoint(const T & t, const V & v) override
         {
            isValid_ = false;
            points_.push_back(std::make_pair(dSeconds(t), v));
         }

      private:
         void validate()
         {
            std::sort(points_.begin(), points_.end(), 
                  [](const std::pair<double, V> & lhs, 
                     const std::pair<double, V> & rhs) -> bool 
                  {return lhs.first < rhs.first;});
            isValid_ = true;
         }
      private:
         bool isValid_;
         std::vector<std::pair<double, V>> points_;
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
