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

#ifndef BUILDING_DOT_H
#define BUILDING_DOT_H

#include <SgtSim/SimComponent.h>
#include <SgtSim/SimParser.h>
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

    class Building : public SimZipAbc, private ZipAbc
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
            Building(const std::string& id) :
                Component(id),
                ZipAbc(Phase::BAL),
                dt_(posix_time::minutes(5)),
                kb_(5 * kwatt / kelvin),
                Cb_(1.0e5 * kjoule / kelvin),
                TbInit_(20.0 * kelvin),
                copCool_(3.0),
                copHeat_(4.0),
                PMaxCool_(20.0 * kwatt),
                PMaxHeat_(20.0 * kwatt),
                Ts_(20.0 * kelvin),
                reqPCool_(0.0),
                reqPHeat_(0.0),
                weather_(nullptr),
                PThInt_(nullptr),
                Tb_(0.0)
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
                return {Complex(-1e-6 * (PHeat() + PCool()), 0.0)}; // Need to convert from W to MW.
            }
            
            /// @}

            /// @name Parameters
            /// @{
            
            /// @brief Timestep, s
            Time dt() const {return dt_;}
            void set_dt(Time val) {dt_ = val; needsUpdate().trigger();}

            /// @brief Thermal conductivity, W/K
            double kb() const {return kb_;}
            void set_kb(double val) {kb_ = val;}

            /// @brief Heat capacity of building, J/K
            double Cb() const {return Cb_;}
            void setCb(double val) {Cb_ = val;}

            /// @brief Initial temperature of building, C
            double TbInit() const {return TbInit_;}
            void setTbInit(double val) {TbInit_ = val; needsUpdate().trigger();}
            
            /// @brief Coefficient of performance for cooling.
            double copCool() const {return copCool_;}
            void setCopCool(double val) {copCool_ = val;}

            /// @brief Coefficient of performance for heating.
            double copHeat() const {return copHeat_;}
            void setCopHeat(double val) {copHeat_ = val; needsUpdate().trigger();}

            /// @brief Maximum cooling electrical power, W.
            double PMaxCool() const {return PMaxCool_;}
            void setPMaxCool(double val) {PMaxCool_ = val;}
            
            /// @brief Maximum heating electrical power, W.
            double PMaxHeat() const {return PMaxHeat_;}
            void setPMaxHeat(double val) {PMaxHeat_ = val;}

            /// @brief Temperature setpoint, C.
            double Ts() const {return Ts_;}
            void setTs(double val) {Ts_ = val;}
            
            /// @brief Requested cooling power, W.
            double reqPCool() const {return reqPCool_;}
            void setReqPCool(double val) {reqPCool_ = val;}
            
            /// @brief Requested heating power, W.
            double reqPHeat() const {return reqPHeat_;}
            void setReqPHeat(double val) {reqPHeat_ = val;}

            /// @brief Weather object.
            void setWeather(std::shared_ptr<Weather> weather) {weather_ = weather; needsUpdate().trigger();}

            /// @brief Time series for internal generated heat. 
            void setPThIntSeries(std::shared_ptr<TimeSeries<Time, double>> PThInt)
            {
                PThInt_ = PThInt; needsUpdate().trigger();
            }
            
            /// @}

            /// @name state
            /// @{
            
            /// @brief Temperature of building.
            double Tb() const {return Tb_;}
            
            /// @}
            
            /// @name Calculated state variables.
            /// @{

            /// @brief External temperature.
            double Te() const {return weather_->temperatureSeries()->value(lastUpdated());}

            /// @brief Extraneous thermal power generated inside building.
            double PThInt() const {return PThInt_->value(lastUpdated());}
 
            /// @brief Actual cooling power, W.
            double PCool() const {return std::min(reqPCool_, PMaxCool_);}
            
            /// @brief Actual heating power, W.
            double PHeat() const {return std::min(reqPHeat_, PMaxHeat_);}
           
            /// @brief HVAC electrical power, W.
            double PHvac() const {return PCool() + PHeat();}

            /// @brief coefficient a in dT/dt = a T + b
            double a() const
            {
                return -kb_ / Cb_;
            }

            /// @brief coefficient b in dT/dt = a T + b
            double b(const Time& t0, const Time& t1) const;

            /// @}
            
            /// @}

        private:
            // Parameters and controls:
            Time dt_; // Timestep.

            // Building thermal parameters:
            double kb_; // Thermal conductivity, W/K.
            double Cb_; // Heat capacity of building, J/K.
            double TbInit_; // Initial temp of building.

            // HVAC performance parameters:
            double copCool_; // HVAC cooling coeff. of perf.
            double copHeat_; // HVAC heating coeff. of perf.
            double PMaxCool_; // Maximum cooling power from grid, W.
            double PMaxHeat_; // Maximum heating power from grid, W.
            
            // HVAC controls:
            double Ts_; // Temperature setpoint, C.
            double reqPCool_; // Requested cooling power.
            double reqPHeat_; // Requested heating power.

            // Extraneous factors:
            std::shared_ptr<Weather> weather_; // For external temperature.
            std::shared_ptr<TimeSeries<Time, double>> PThInt_; // Extraneous heat generated in building.

            // State.
            double Tb_; // Building temperature, C.
    };

    class BuildingParserPlugin : public SimParserPlugin
    {
        public:
            virtual const char* key() override
            {
                return "building";
            }

        public:
            virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
}
#endif // BUILDING_DOT_H
