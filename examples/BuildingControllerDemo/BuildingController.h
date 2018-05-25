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

#ifndef BUILDING_CONTROLLER_DOT_H
#define BUILDING_CONTROLLER_DOT_H

#include <SgtCore/Parser.h>

#include <SgtSim/Battery.h>
#include <SgtSim/Heartbeat.h>
#include <SgtSim/SimNetwork.h>
#include <SgtSim/SimParser.h>
#include <SgtSim/Simulation.h>
#include <SgtSim/SolarPv.h>

#include <gurobi_c++.h>

namespace Sgt
{
    class Building;

    class BuildingController : public Heartbeat
    {
        friend class BuildingControllerParserPlugin;

        public:

        using LoadSeries = TimeSeries<Time, arma::Col<Complex>>; 
        using PriceSeries = TimeSeries<Time, double>;
        using TempSeries = TimeSeries<Time, double>;

        BuildingController(const std::string& id, const Time& dt);

        void setBatt(const SimComponentPtr<Battery>& batt);

        void setBuild(const SimComponentPtr<Building>& build);

        void setSolar(const SimComponentPtr<SolarPv>& solar);

        void setLoadSeries(const ConstTimeSeriesPtr<LoadSeries>& loadSeries)
        {
            loadSeries_ = loadSeries;
        }

        void setPriceSeries(const ConstTimeSeriesPtr<PriceSeries>& priceSeries)
        {
            priceSeries_ = priceSeries;
        }

        void setTExtSeries(const ConstTimeSeriesPtr<TempSeries>& TExtSeries)
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

        virtual void updateState(const Time& t) override;

        private:

        SimComponentPtr<Battery> batt_;
        SimComponentPtr<Building> build_;
        SimComponentPtr<SolarPv> solar_;

        ConstTimeSeriesPtr<LoadSeries> loadSeries_;
        ConstTimeSeriesPtr<PriceSeries> priceSeries_;
        ConstTimeSeriesPtr<TempSeries> TExtSeries_;

        double feedInTariff_; // Objective paid for power injected back into grid.
        double comfortFactor_; // Objective per degree away from setpoint.
        double TSetp_; // Temperature setpoint.
        double TMaxDev_; // Max allowable deviation from setpoint before incurring cost.

        GRBEnv env_;
    };

    class BuildingControllerParserPlugin : public SimParserPlugin
    {
        public:

        virtual const char* key() const override
        {
            return "building_controller";
        }

        public:

        virtual void parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const override;
    };
};

#endif // PV_DEMO_CONTROLLER_DOT_H
