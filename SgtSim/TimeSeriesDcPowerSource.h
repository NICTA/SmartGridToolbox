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

#ifndef TIME_SERIES_DC_POWER_SOURCE_DOT_H
#define TIME_SERIES_DC_POWER_SOURCE_DOT_H

#include <SgtSim/DcPowerSource.h>
#include <SgtSim/Heartbeat.h>
#include <SgtSim/TimeSeries.h>
#include <SgtSim/SimNetwork.h> 

namespace Sgt
{
    class DcPowerSource;

    class TimeSeriesDcPowerSource: public DcPowerSourceAbc, public Heartbeat
    {
        public:
        static const std::string& sComponentType()
        {
            static std::string result("time_series_dc_power_source");
            return result;
        }

        public:

        TimeSeriesDcPowerSource(const std::string& id, const ConstTimeSeriesPtr<TimeSeries<Time, double>>& series,
                const Time& dt, double scaleFactor = 1.0) :
            Component(id),
            Heartbeat(dt),
            series_(series),
            scaleFactor_(scaleFactor)
        {
            // Empty.
        }

        virtual const std::string& componentType() const override
        {
            return sComponentType();
        }

        virtual double requestedPDc() const override
        {
            return scaleFactor_ * series_->value(lastUpdated());
        }

        private:
        ConstTimeSeriesPtr<TimeSeries<Time, double>> series_;
        double scaleFactor_{1.0};
    };
}

#endif // TIME_SERIES_DC_POWER_SOURCE_DOT_H
