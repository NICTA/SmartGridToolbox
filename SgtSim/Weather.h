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
#include <SgtSim/SolarGeom.h>

#include <SgtCore/Common.h>

namespace Sgt
{
    struct Weather
    {
        LatLong latLong{greenwich};
        double elevation{0.0};
        std::function<double (const Time&)> temperature{[](const Time&){return 20.0;}}; // Centigrade.
        std::function<Irradiance (const Time&)> irradiance{[](const Time&)->Irradiance
            {return Irradiance{{{0.0, 0.0, 0.0}}, 0.0, 0.0};}};
        std::function<Array<double, 3> (const Time&)> windVector{
            [](const Time&)->Array<double, 3>{return {{0.0, 0.0, 0.0}};}};
    };

    class SimWeather : public Heartbeat
    {
        public:
        
            Weather weather;

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("weather");
                return result;
            }

        /// @}

        /// @name Lifecycle:
        /// @{

            SimWeather(const std::string& id) :
                Component(id),
                Heartbeat(id, posix_time::minutes(5))
            {
                // Empty.
            }

            virtual ~SimWeather() = default;

        /// @name Component virtual overridden member functions.
        /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

        /// @}

        /// @name Weather specific member functions.
        /// @{
            
            double temperature()
            {
                return weather.temperature(lastUpdated());
            }
            
            Irradiance irradiance()
            {
                return weather.irradiance(lastUpdated());
            }
            
            Array<double, 3> windVector()
            {
                return weather.windVector(lastUpdated());
            }

        /// @}
    };
}

#endif // WEATHER_DOT_H
