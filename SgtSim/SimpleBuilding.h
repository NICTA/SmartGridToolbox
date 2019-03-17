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

#ifndef SIMPLE_BUILDING_DOT_H
#define SIMPLE_BUILDING_DOT_H

#include <SgtSim/SimComponent.h>
#include <SgtSim/TimeSeries.h>
#include <SgtSim/SimNetwork.h>
#include <SgtSim/Weather.h>

#include <SgtCore/Common.h>

#include<string>

namespace Sgt
{
    enum class HvacMode
    {
        COOLING,
        HEATING,
        OFF
    };

    class Zip;

    class SimpleBuilding : public SimZip
    {
        /// @name Static member functions:
        /// @{

        public:

        static const std::string& sComponentType()
        {
            static std::string result("simple_building");
            return result;
        }

        /// @}

        /// @name Lifecycle:
        /// @{

        public:
        SimpleBuilding(const std::string& id, const ComponentPtr<Zip>& zip) :
            Component(id),
            SimZip(zip),
            dt_(minutes(5)),
            kb_(5 * kwatt / kelvin),
            Cb_(1.0e5 * kjoule / kelvin),
            TbInit_(20.0 * kelvin),
            kh_(10.0 * kwatt / kelvin),
            copCool_(3.0),
            copHeat_(4.0),
            PMax_(20.0 * kwatt),
            Ts_(20.0 * kelvin),
            weather_(nullptr),
            dQg_(nullptr),
            Tb_(0.0),
            mode_(HvacMode::OFF),
            cop_(0.0),
            isMaxed_(false),
            Ph_(0.0),
            dQh_(0.0)
        {
        }

        /// @}

        /// @name Component virtual overridden member functions.
        /// @{

        public:

        virtual const std::string& componentType() const override
        {
            return sComponentType();
        }

        // virtual json toJson() const override; // TODO

        /// @}

        /// @name SimComponent virtual overridden member functions.
        /// @{

        public:

        virtual Time validUntil() const override {return lastUpdated() + dt_;}

        protected:

        virtual void initializeState() override;
        virtual void updateState(const Time& t) override;

        /// @}

        public:

        /// @name Parameters
        /// @{

        /// @brief Timestep, s
        Time dt() {return dt_;}
        void set_dt(Time val) {dt_ = val; needsUpdate().trigger();}

        /// @brief Thermal conductivity, W/K
        double kb() {return kb_;}
        void set_kb(double val) {kb_ = val;}

        /// @brief Heat capacity of building, J/K
        double Cb() {return Cb_;}
        void setCb(double val) {Cb_ = val;}

        /// @brief Initial temperature of building, C
        double TbInit() {return TbInit_;}
        void setTbInit(double val) {TbInit_ = val; needsUpdate().trigger();}

        /// @brief HVAC PID parameter, W/K.
        double kh() {return kh_;}
        void set_kh(double val) {kh_ = val;}

        /// @brief Coefficient of performance for cooling.
        double copCool() {return copCool_;}
        void setCopCool(double val) {copCool_ = val;}

        /// @brief Coefficient of performance for heating.
        double copHeat() {return copHeat_;}
        void setCopHeat(double val) {copHeat_ = val; needsUpdate().trigger();}

        /// @brief Maximum HVAC power, W.
        double PMax() {return PMax_;}
        void setPMax(double val) {PMax_ = val;}

        /// @brief Temperature setpoint, C.
        double Ts() {return Ts_;}
        void setTs(double val) {Ts_ = val;}

        /// @brief Weather object.
        void setWeather(const ConstSimComponentPtr<Weather>& weather) 
        {
            weather_ = weather; needsUpdate().trigger();
        }

        /// @brief Time series for internal generated heat. 
        void set_dQgSeries(const ConstTimeSeriesPtr<TimeSeries<Time, double>>& dQg) 
        {
            dQg_ = dQg; needsUpdate().trigger();
        }

        /// @}

        /// @name state
        /// @{

        /// @brief Temperature of building.
        double Tb() {return Tb_;}

        /// @brief External temperature.
        double Te() {return weather_->model.temperature(lastUpdated());}

        /// @brief Extraneous thermal power generated inside building.
        double dQg() {return dQg_->value(lastUpdated());}

        /// @brief HVAC mode (COOLING/HEATING/OFF).
        HvacMode mode() {return mode_;}

        /// @brief HVAC coefficient of performance.
        double cop() {return cop_;}

        /// @brief Is HVAC maxed out? 
        double isMaxed() {return isMaxed_;}

        /// @brief HVAC electrical power, W.
        double Ph() {return Ph_;}

        /// @brief HVAC thermal power, +ve = heating.
        double dQh() {return dQh_;}

        /// @}

        /// @name My private member functions.
        /// @{

        private:
        virtual arma::Mat<Complex> SConst() const
        {
            return {Complex(1e-6 * Ph_, 0.0)}; // Need to convert from W to MW.
        }

        void setOperatingParams(Time t);

        /// @}

        private:
        // Parameters and controls.
        Time dt_;                                                       // Timestep.
        double kb_;                                                     // Thermal conductivity, W/K.
        double Cb_;                                                     // Heat capacity of building, J/K.
        double TbInit_;                                                 // Initial temp of building.
        double kh_;                                                     // HVAC PID parameter, W/K.
        double copCool_;                                                // HVAC cooling coeff. of perf.
        double copHeat_;                                                // HVAC heating coeff. of perf.
        double PMax_;                                                   // HVAC max power, W.
        double Ts_;                                                     // HVAC set_point, C.

        ConstSimComponentPtr<Weather> weather_;                         // For external temperature.
        ConstTimeSeriesPtr<TimeSeries<Time, double>> dQg_;              // Extra heat -> building.

        // State.
        double Tb_;                                                     // Building temperature, C.
        // Operating parameters.
        HvacMode mode_;                                                 // Cooling or heating?
        double cop_;                                                    // Depends on mode.
        bool isMaxed_;                                                  // On maximum power?
        double Ph_;                                                     // Power drawn from grid by HVAC.
        double dQh_;                                                    // Thermal output, +ve = heating.
    };
}
#endif // SIMPLE_BUILDING_DOT_H
