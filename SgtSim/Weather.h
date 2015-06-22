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

#ifndef WEATHER_DOT_H
#define WEATHER_DOT_H

#include <SgtSim/Heartbeat.h>
#include <SgtSim/Sun.h>
#include <SgtSim/TimeSeries.h>

namespace Sgt
{
    struct SolarIrradiance
    {
        Array<double, 3> direct;
        double horizontalDiffuse;
    };

    class Weather : public Heartbeat
    {
        /// @name Static member functions:
        /// @{

        public:

            static const std::string& sComponentType()
            {
                static std::string result("weather");
                return result;
            }

        /// @}

        /// @name Lifecycle:
        /// @{

            Weather(const std::string& id) :
                Heartbeat(id, posix_time::minutes(5)),
                latLong_(greenwich),
                temperatureSeries_(nullptr),
                cloudCoverSeries_(nullptr)
            {
                // Empty.
            }

        /// @name Component virtual overridden functions.
        /// @{

        public:

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

        /// @}

        /// @name Weather specific member functions.
        /// @{

        public:

            void setLatLong(const LatLong& latLong) {latLong_ = latLong; needsUpdate().trigger();}

            std::shared_ptr<const TimeSeries<Time, double>> temperatureSeries() const
            {
                return temperatureSeries_;
            }
            void setTemperatureSeries(std::shared_ptr<const TimeSeries<Time, double>> series)
            {
                temperatureSeries_ = series;
                needsUpdate().trigger();
            }
            double temperature() const
            {
                return temperatureSeries_->value(lastUpdated());
            }

            std::shared_ptr<const TimeSeries<Time, double>> cloudCoverSeries() const
            {
                return cloudCoverSeries_;
            }
            void setCloudCoverSeries(std::shared_ptr<const TimeSeries<Time, double>> series)
            {
                cloudCoverSeries_ = series;
                needsUpdate().trigger();
            }
            double cloudCover() const
            {
                return cloudCoverSeries_->value(lastUpdated());
            }

            double solarPower(SphericalAngles planeNormal, double planeArea) const;

            SolarIrradiance irradiance(const Time& tm) const;

        /// @}

        private:
            LatLong latLong_;
            std::shared_ptr<const TimeSeries<Time, double>> temperatureSeries_;
            std::shared_ptr<const TimeSeries<Time, double>> cloudCoverSeries_;
    };
}

#endif // WEATHER_DOT_H
