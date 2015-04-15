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
                latLong_(Greenwich),
                temperatureSeries_(nullptr),
                cloudCoverSeries_(nullptr)
            {
                // Empty.
            }

        /// @name ComponentInterface virtual overridden functions.
        /// @{

        public:

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            // virtual void print(std::ostream& os) const override; TODO

        /// @}

        /// @name Overridden member functions from SimComponent.
        /// @{

        public:

            // virtual Time validUntil() const override;

        protected:

            // virtual void initializeState() override;
            virtual void updateState(Time t) override;

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

            const SolarIrradiance& irradiance()
            {
                return irradiance_;
            }

        protected:
            SolarIrradiance unaveragedIrradiance(const Time& tm) const;

        /// @}

        private:
            LatLong latLong_;
            std::shared_ptr<const TimeSeries<Time, double>> temperatureSeries_;
            std::shared_ptr<const TimeSeries<Time, double>> cloudCoverSeries_;

            SolarIrradiance prevIrradiance_;
            SolarIrradiance irradiance_;
    };
}

#endif // WEATHER_DOT_H
