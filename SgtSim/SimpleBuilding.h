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

#ifndef SIMPLE_BUILDING_DOT_H
#define SIMPLE_BUILDING_DOT_H

#include <SgtSim/SimComponent.h>
#include <SgtSim/SimZip.h>
#include <SgtSim/TimeSeries.h>
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

    class SimpleBuilding : public SimZipAbc, private ZipAbc
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
            SimpleBuilding(const std::string& id) :
                Component(id),
                ZipAbc(Phase::BAL),
                dt_(posix_time::minutes(5)),
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

            // virtual void print(std::ostream& os) const override; TODO

            /// @}

            /// @name SimComponent virtual overridden member functions.
            /// @{

        public:

            virtual Time validUntil() const override {return lastUpdated() + dt_;}

        protected:

            virtual void initializeState() override;
            virtual void updateState(Time t) override;

            /// @}
        
            /// @name SimZipAbc virtual overridden member functions.
            /// @{

        public:

            virtual std::shared_ptr<const ZipAbc> zip() const override
            {
                return shared<const ZipAbc>();
            }

            virtual std::shared_ptr<ZipAbc> zip() override
            {
                return shared<ZipAbc>();
            }

            /// @}

            /// @name ZipAbc virtual overridden member functions.
            /// @{

            virtual arma::Col<Complex> SConst() const override
            {
                return {Complex(-1e-6 * Ph_, 0.0)}; // Need to convert from W to MW.
            }
            
            /// @}

            // Parameters:
            Time dt() {return dt_;}
            void set_dt(Time val) {dt_ = val; needsUpdate().trigger();}

            double kb() {return kb_;}
            void set_kb(double val) {kb_ = val;}

            double Cb() {return Cb_;}
            void setCb(double val) {Cb_ = val;}

            double TbInit() {return TbInit_;}
            void setTbInit(double val) {TbInit_ = val; needsUpdate().trigger();}

            double kh() {return kh_;}
            void set_kh(double val) {kh_ = val;}

            double copCool() {return copCool_;}
            void setCopCool(double val) {copCool_ = val;}

            double copHeat() {return copHeat_;}
            void setCopHeat(double val) {copHeat_ = val; needsUpdate().trigger();}

            double PMax() {return PMax_;}
            void setPMax(double val) {PMax_ = val;}

            double Ts() {return Ts_;}
            void setTs(double val) {Ts_ = val;}

            void setWeather(std::shared_ptr<Weather> weather) {weather_ = weather; needsUpdate().trigger();}

            void set_dQgSeries(std::shared_ptr<TimeSeries<Time, double>> dQg) {dQg_ = dQg; needsUpdate().trigger();}

            double Tb() {return Tb_;}

            double Te() {return weather_->temperatureSeries()->value(lastUpdated());}

            double dQg() {return dQg_->value(lastUpdated());}

            HvacMode mode() {return mode_;}

            double cop() {return cop_;}

            double isMaxed() {return isMaxed_;}

            double Ph() {return Ph_;}

            double dQh() {return dQh_;}

            /// @}

            /// @name My private member functions.
            /// @{

        private:
            void setOperatingParams(Time t);

            /// @}

        private:
            // Parameters and controls.
            Time dt_;                                       // Timestep.
            double kb_;                                     // Thermal conductivity, W/K.
            double Cb_;                                     // Heat capacity of building, J/K.
            double TbInit_;                                 // Initial temp of building.
            double kh_;                                     // HVAC PID parameter, W/K.
            double copCool_;                                // HVAC cooling coeff. of perf.
            double copHeat_;                                // HVAC heating coeff. of perf.
            double PMax_;                                   // HVAC max power, W.
            double Ts_;                                     // HVAC set_point, C.

            std::shared_ptr<Weather> weather_;              // For external temperature.
            std::shared_ptr<TimeSeries<Time, double>> dQg_; // Extra heat -> building.

            // State.
            double Tb_;                                     // Building temperature, C.
            // Operating parameters.
            HvacMode mode_;                                 // Cooling or heating?
            double cop_;                                    // Depends on mode.
            bool isMaxed_;                                  // On maximum power?
            double Ph_;                                     // Power drawn from grid by HVAC.
            double dQh_;                                    // Thermal output, +ve = heating.
    };
}
#endif // SIMPLE_BUILDING_DOT_H
