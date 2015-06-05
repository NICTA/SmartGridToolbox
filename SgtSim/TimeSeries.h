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
    /// @ingroup Core
    template<typename T, typename V>
    class TimeSeries : public TimeSeriesBase
    {
        public:
            virtual V value(const T& t) const = 0;
    };

    /// @brief Time series for a constant function of time. 
    /// @ingroup Core
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
    /// @ingroup Core
    template<typename T, typename V>
    class DataTimeSeries : public TimeSeries<T, V>
    {
        public:
            virtual void addPoint(const T& t, const V& v) = 0;
    };

    /// @brief TimeSeries that changes in a stepwise manner between tabulated times. 
    /// @ingroup Core
    template<typename T, typename V>
    class StepwiseTimeSeries : public DataTimeSeries<T, V>
    {
        public:
            virtual V value(const T& t) const override
            {
                auto pos = points_.upper_bound(dSeconds(t));
                if (pos != points_.begin())
                {
                    --pos;
                }
                return pos->second;

            }

            virtual void addPoint(const T& t, const V& v) override
            {
                points_[dSeconds(t)] = v;
            }

        private:
            std::map<double, V> points_;
    };

    /// @brief TimeSeries that uses linear interpolation between tabulated times.
    /// @ingroup Core
    template<typename T, typename V>
    class LerpTimeSeries : public DataTimeSeries<T, V>
    {
        public:
            virtual V value(const T& t) const override
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
                    auto pos1 = pos2;
                    --pos1;
                    return pos1->second + (pos2->second - pos1->second) * (td - pos1->first) /
                        (pos2->first - pos1->first);
                }
            }

            virtual void addPoint(const T& t, const V& v) override
            {
                points_[dSeconds(t)] = v;
            }

        private:
            std::map<double, V> points_;
    };

#if 0 // TODO: redo spline due to license issues.
    /// @brief TimeSeries that uses spline interpolation between tabulated times.
    /// @ingroup Core
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
    /// @ingroup Core
    template<typename T, typename V>
    class FunctionTimeSeries : public TimeSeries<T, V>
    {
        public:
            FunctionTimeSeries<T, V>(const std::function<V (T)>& func) :
                func_(func)
            {
                // Empty.
            }

            virtual double value(const T& t) const override
            {
                return func_(t);
            }

        private:

            std::function<V (T)> func_;
    };
}

#endif // TIME_SERIES_DOT_H
