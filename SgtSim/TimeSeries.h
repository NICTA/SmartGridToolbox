// Copyright 2015-2016 National ICT Australia Limited (NICTA)
// Copyright 2016-2019 The Australian National University
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

#include <SgtCore/Common.h>

#include <map>

namespace Sgt
{
    template<typename T> struct TsTraits
    {
        static double toDouble(const T& t) {return static_cast<double>(t);}
    };

    template<> struct TsTraits<double>
    {
        static double toDouble(const double& t) {return t;}
    };

    template<> struct TsTraits<Time>
    {
        static double toDouble(const Time& t) {return dSeconds(t);}
    };

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
        DataTimeSeries(const V& defaultV) : defaultV_(defaultV) {}

        virtual void addPoint(const T& t, const V& v) {points_[t] = v;}

        auto begin() const {return points_.begin();};
        auto end() const {return points_.end();};
        auto rbegin() const {return points_.rbegin();};
        auto rend() const {return points_.rend();};

        /// @brief Iterator to element with largest key <= t, or rend if no such key exists.
        auto lowerBound(const T& t) const
        {
            // NOTE: Careful with this code.
            // See the definition of map::upper_bound and map::lower_bound, which might be different to
            // what one would expect.
            // Also note, const_reverse_iterator(iterator) constructor shifts back by 1.
            return typename decltype(points_)::const_reverse_iterator(points_.upper_bound(t)); 
        }
        /// @brief Iterator to element with smallest key >= t, or rend if no such key exists.
        auto upperBound(const T& t) const
        {
            // NOTE: Careful with this code.
            // See the definition of map::upper_bound and map::lower_bound, which might be different to
            // what one would expect.
            return points_.lower_bound(t);
        }

        void removePoint(const typename std::map<T, V>::const_iterator& it) {points_.erase(it);}
        void removePoints(const typename std::map<T, V>::const_iterator& a,
                const typename std::map<T, V>::const_iterator& b) {points_.erase(a, b);}

        protected:
        std::map<T, V> points_;
        V defaultV_;
    };

    /// @brief TimeSeries that changes in a stepwise manner between tabulated times. 
    /// @ingroup SimCore
    template<typename T, typename V>
        class StepwiseTimeSeries : public DataTimeSeries<T, V>
    {
        protected:
        using DataTimeSeries<T, V>::points_;
        using DataTimeSeries<T, V>::defaultV_;

        public:
        StepwiseTimeSeries(const V& defaultV) : DataTimeSeries<T, V>(defaultV) {}

        virtual V value(const T& t) const override
        {
            return (points_.size() == 0 || t < points_.begin()->first || t > points_.rbegin()->first)
                ? defaultV_
                : (--points_.upper_bound(t))->second; // Highest point <= t.
        }
    };

    /// @brief TimeSeries that uses linear interpolation between tabulated times.
    /// @ingroup SimCore
    template<typename T, typename V>
        class LerpTimeSeries : public DataTimeSeries<T, V>
    {
        protected:
        using DataTimeSeries<T, V>::points_;
        using DataTimeSeries<T, V>::defaultV_;

        public:
        LerpTimeSeries(const V& defaultV) : DataTimeSeries<T, V>(defaultV) {}

        virtual V value(const T& t) const override
        {
            if (points_.size() == 0 || t < points_.begin()->first || t > points_.rbegin()->first) return defaultV_;

            auto pos2 = points_.upper_bound(t);
            // pos2 -> first point > s. It can't be begin, but could be end (if t == last point).
            if (pos2 == points_.end())
            {
                return points_.rbegin()->second;
            }
            else
            {
                // We now know that t is strictly inside the range.
                auto pos1 = pos2; --pos1;
                return pos1->second + (pos2->second - pos1->second) * TsTraits<T>::toDouble(t - pos1->first) /
                    TsTraits<T>::toDouble(pos2->first - pos1->first);
            }
        }
    };

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

// TODO: Reinstate SplineTimeSeries that was removed due to license issues.

#endif // TIME_SERIES_DOT_H
