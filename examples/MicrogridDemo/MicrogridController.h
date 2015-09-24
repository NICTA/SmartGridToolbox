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

#ifndef MICROGRID_DEMO_CONTROLLER_DOT_H
#define MICROGRID_DEMO_CONTROLLER_DOT_H

#include <SgtCore/Parser.h>

#include <SgtSim/Battery.h>
#include <SgtSim/Heartbeat.h>
#include <SgtSim/SimBus.h>
#include <SgtSim/SimParser.h>
#include <SgtSim/Simulation.h>
#include <SgtSim/SolarPv.h>

extern "C" {
#include "gurobi_c.h"
};

namespace Sgt
{
    class Building;

    class MicrogridController : public Heartbeat
    {
        public:

            using LoadSeries = TimeSeries<Time, arma::Col<Complex>>; 
            using PriceSeries = TimeSeries<Time, double>;
            using TempSeries = TimeSeries<Time, double>;

            MicrogridController(const std::string& id, const Time& dt);

            void setBatt(std::shared_ptr<Battery> batt);
            
            void setBuild(std::shared_ptr<Building> build);
            
            void setSolar(std::shared_ptr<SolarPv> solar)
            {
                solar_ = solar;
            }

            void setLoadSeries(std::shared_ptr<const LoadSeries> loadSeries)
            {
                loadSeries_ = loadSeries;
            }
            
            void setPriceSeries(std::shared_ptr<const PriceSeries> priceSeries)
            {
                priceSeries_ = priceSeries;
            }
            
            void setTExtSeries(std::shared_ptr<const TempSeries> TExtSeries)
            {
                TExtSeries_ = TExtSeries;
            }

            void setFeedInTariff(double feedInTariff)
            {
                feedInTariff_ = feedInTariff;
            }
            
            void setComfortFactor(double comfortFactor)
            {
                comfortFactor_ = comfortFactor;
            }
            
            void setTSetp(double TSetp)
            {
                TSetp_ = TSetp;
            }
            
            void setTMaxDev(double TMaxDev)
            {
                TMaxDev_ = TMaxDev;
            }

        protected:
            virtual void updateState(Time t) override;

        private:
            std::shared_ptr<Battery> batt_;
            std::shared_ptr<Building> build_;
            std::shared_ptr<SolarPv> solar_;

            std::shared_ptr<const LoadSeries> loadSeries_;
            std::shared_ptr<const PriceSeries> priceSeries_;
            std::shared_ptr<const TempSeries> TExtSeries_;

            double feedInTariff_; // Objective paid for power injected back into grid.
            double comfortFactor_; // Objective per degree away from setpoint.
            double TSetp_; // Temperature setpoint.
            double TMaxDev_; // Max allowable deviation from setpoint before incurring cost.

            GRBenv* env{NULL};
    };

    class MicrogridControllerParserPlugin : public SimParserPlugin
    {
        public:
            virtual const char* key() override
            {
                return "microgrid_controller";
            }

        public:
            virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
};

#endif // PV_DEMO_CONTROLLER_DOT_H
