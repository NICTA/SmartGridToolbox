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

#ifndef WEATHER_DOT_H
#define WEATHER_DOT_H

#include <SgtSim/Heartbeat.h>
#include <SgtSim/SolarGeom.h>
#include <SgtSim/Sun.h>
#include <SgtSim/TimeSeries.h>

#include <SgtCore/Common.h>

namespace Sgt
{
    struct WeatherModel
    {
        LatLong latLong{greenwich.lat_, greenwich.long_};
        double altitude{0.0};

        std::function<double (const Time&)> temperature{[](const Time&){return 20.0;}}; // Centigrade.

        std::function<Irradiance (const Time&)> irradiance{[](const Time&)->Irradiance{
            return {{{0.0, 0.0, 0.0}}, 0.0, 0.0};}};

        std::function<arma::Col<double>::fixed<3> (const Time&)> windVector{
            [](const Time&)->arma::Col<double>::fixed<3>{return {{0.0, 0.0, 0.0}};}};

        std::function<arma::Col<double>::fixed<3> (const Time&, const SphericalAngles&)> cloudAttenuationFactors{
            [](const Time&, const SphericalAngles& angs)->arma::Col<double>::fixed<3>{return {{1.0, 1.0, 1.0}};}}; 
        // No attenuation for any component by default.

        // Utility functions:
        template<typename T> void setTemperature(const T& f)
        {
            temperature = [f](const Time& t){return f(t);};
        }
        void setTemperatureSeries(const ConstTimeSeriesPtr<TimeSeries<Time, double>>& series)
        {
            temperature = [series](const Time& t){return series->value(t);};
        }
        void setTemperatureConst(double val)
        {
            temperature = [val](const Time& t){return val;};
        }

        template<typename T> void setIrradiance(const T& f)
        {
            irradiance = [f](const Time& t){return f(t);};
        }
        void setIrradianceSeries(const ConstTimeSeriesPtr<TimeSeries<Time, arma::Col<double>>>& series)
        {
            irradiance = [series](const Time& t)->Irradiance{
                auto val = series->value(t); return {{{val(0), val(1), val(2)}}, val(3), val(4)};};
        }
        void setIrradianceConst(const Irradiance& val)
        {
            irradiance = [val](const Time& t){return val;};
        }
        void setIrradianceToSunModel()
        {
            irradiance = [this](const Time& t)->Irradiance{return sunModelIrr(t);};
        }

        template<typename T> void setWindVector(const T& f)
        {
            windVector = [f](const Time& t){return f(t);};
        }
        void setWindVectorSeries(const ConstTimeSeriesPtr<TimeSeries<Time, arma::Col<double>>>& series)
        {
            windVector = [series](const Time& t)->arma::Col<double>::fixed<3>{
                auto val = series->value(t); return {{val(0), val(1), val(2)}};};
        }
        void setWindVectorConst(const arma::Col<double>::fixed<3>& val)
        {
            windVector = [val](const Time& t){return val;};
        }

        template<typename T> void setCloudAttenuationFactors(const T& f)
        {
            cloudAttenuationFactors = [f](const Time& t, const SphericalAngles&){return f(t);};
        }
        void setCloudAttenuationFactorsSeries(const ConstTimeSeriesPtr<TimeSeries<Time, arma::Col<double>>>& series)
        {
            cloudAttenuationFactors = [series](const Time& t, const SphericalAngles&)->arma::Col<double>::fixed<3>{
                auto val = series->value(t); return {{val(0), val(1), val(2)}};};
        }
        void setCloudAttenuationFactorsConst(const arma::Col<double>::fixed<3>& val)
        {
            cloudAttenuationFactors = [val](const Time& t, const SphericalAngles&){return val;};
        }

        private:

        Irradiance sunModelIrr(const Time& t);
    };

    class Weather : public Heartbeat
    {
        public:

        WeatherModel model;

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

        Weather(const std::string& id) :
            Component(id),
            Heartbeat(minutes(5))
        {
            // Empty.
        }

        virtual ~Weather() = default;

        /// @name Component virtual overridden member functions.
        /// @{

        virtual const std::string& componentType() const override
        {
            return sComponentType();
        }

        /// @}

        /// @name Weather specific member functions.
        /// @{

        double temperature() const
        {
            return model.temperature(lastUpdated());
        }

        Irradiance irradiance() const
        {
            return model.irradiance(lastUpdated());
        }

        arma::Col<double>::fixed<3> windVector() const
        {
            return model.windVector(lastUpdated());
        }

        arma::Col<double>::fixed<3> cloudAttenuationFactors() const
        {
            return model.cloudAttenuationFactors(lastUpdated(), sunPos(lastUpdated(), model.latLong));
        }

        /// @}
    };
    }

#endif // WEATHER_DOT_H
