// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TIME_SERIES_DOT_H
#define TIME_SERIES_DOT_H

#include <SgtCore/ComponentCollection.h>
#if 0 // TODO: redo spline due to license issues.
#include <SgtCore/Spline.h>
#endif

#include <map>

namespace Sgt
{
    class TimeSeriesBase
    {
        public:
            virtual ~TimeSeriesBase() = default;
    };

    /// @brief Base class for time series objects: functions of time.
    /// @ingroup SimCore
    template<typename T, typename V>
    class TimeSeries : public TimeSeriesBase
    {
        public:
            using TimeTipe = T;
            using ValType = V;

            virtual V value(const T& t) const = 0;
    };

    /// @brief Time series for a constant function of time. 
    /// @ingroup SimCore
    template<typename T, typename V>
    class ConstTimeSeries : public TimeSeries<T, V>
    {
        public:
            ConstTimeSeries(const V& val) {val_ = val;}

            virtual V value(const T& t) const {return val_;}

        private:
            V val_;
    };

    /// @brief Base class for TimeSeries object that stores a time/value table.  
    /// @ingroup SimCore
    template<typename T, typename V>
    class DataTimeSeries : public TimeSeries<T, V>
    {
        public:
            virtual void addPoint(const T& t, const V& v) = 0;
    };

    /// @brief TimeSeries that changes in a stepwise manner between tabulated times. 
    /// @ingroup SimCore
    template<typename T, typename V>
    class StepwiseTimeSeries : public DataTimeSeries<T, V>
    {
        public:
            StepwiseTimeSeries(const V& defaultV) : defaultV_(defaultV) {}

            virtual V value(const T& t) const override
            {
                double s = dSeconds(t);
                return (s < points_.begin()->first || s > points_.rbegin()->first)
                    ? defaultV_
                    : (--points_.upper_bound(dSeconds(t)))->second; // Highest point <= s.
            }

            virtual void addPoint(const T& t, const V& v) override
            {
                points_[dSeconds(t)] = v;
            }

        private:
            std::map<double, V> points_;
            V defaultV_;
    };

    /// @brief TimeSeries that uses linear interpolation between tabulated times.
    /// @ingroup SimCore
    template<typename T, typename V>
    class LerpTimeSeries : public DataTimeSeries<T, V>
    {
        public:
            LerpTimeSeries(const V& defaultV) : defaultV_(defaultV) {}

            virtual V value(const T& t) const override
            {
                double s = dSeconds(t);
                if (s < points_.begin()->first || s > points_.rbegin()->first) return defaultV_;

                auto pos2 = points_.upper_bound(s);
                // pos2 -> first point > s. It can't be begin, but could be end (if s -> last point)..
                if (pos2 == points_.end())
                {
                    return points_.rbegin()->second;
                }
                else
                {
                    // We now know that s is strictly inside the range.
                    auto pos1 = pos2; --pos1;
                    return pos1->second + (pos2->second - pos1->second) * (s - pos1->first) /
                        (pos2->first - pos1->first);
                }
            }

            virtual void addPoint(const T& t, const V& v) override
            {
                points_[dSeconds(t)] = v;
            }

        private:
            std::map<double, V> points_;
            V defaultV_;
    };

#if 0 // TODO: redo spline due to license issues.
    /// @brief TimeSeries that uses spline interpolation between tabulated times.
    /// @ingroup SimCore
    template<typename T>
    class SplineTimeSeries : public DataTimeSeries<T, double>
    {
        public:
            virtual double value(const T& t) const override
            {
                return spline_(dSeconds(t));
            }

            virtual void addPoint(const T& t, const double& v) override
            {
                spline_.addPoint(dSeconds(t), v);
            }

        private:

            mutable Spline spline_;
    };
#endif

    /// @brief TimeSeries that uses a std::function to calculate values.
    /// @ingroup SimCore
    template<typename T, typename V>
    class FunctionTimeSeries : public TimeSeries<T, V>
    {
        public:
            FunctionTimeSeries<T, V>(const std::function<V (T)>& func) :
                func_(func)
            {
                // Empty.
            }

            virtual V value(const T& t) const override
            {
                return func_(t);
            }

        private:

            std::function<V (T)> func_;
    };
    
    template<typename T> using TimeSeriesPtr = ComponentPtr<TimeSeriesBase, T>;
    template<typename T> using ConstTimeSeriesPtr = ConstComponentPtr<TimeSeriesBase, T>;
}

#endif // TIME_SERIES_DOT_H
