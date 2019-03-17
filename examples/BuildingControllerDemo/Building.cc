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

#include "Building.h"

#include <SgtSim/Simulation.h>

namespace Sgt
{
    void Building::initializeState()
    {
        Tb_ = TbInit_;
    }

    void Building::updateState(const Time& t)
    {
        double dt = lastUpdated() == TimeSpecialValues::neg_infin ? 0 : dSeconds(t - lastUpdated());

        // Solve dT/dt = -(kb/Cb)(T - TExt) + copHeat PHeat/Cb - copCool PCool/Cb
        // => dT/dt = a T + b
        // a = -kb/Cb
        // b = (kb TExt + copHeat PHeat - copCool PCool) / Cb
        //
        // Solution is T(t) = (T0 + b/a) exp(at) - b/a 
        //                  = d T0 + (1 - d) c
        //
        // where d = exp(at) = exp(-(kb/Cb)t)
        //       c = -b/a = TExt + (copHeat / kb) PHeat - (copCool / kb) PCool
        //
        // Approximate TExt by its average over the interval.
       
        double dSv = d(dt);
        double cSv = c(lastUpdated(), t);
        Tb_ = dSv * Tb_ + (1.0 - dSv) * cSv;

        zip()->setSConst({std::initializer_list<Complex>{Complex(1e-6 * (PHeat() + PCool()), 0.0)}});
            // Need to convert from W to MW.

    }

    void Building::setWeather(const ConstSimComponentPtr<Weather>& weather)
    {
        weather_ = weather;
        dependsOn(weather, true);
    }

    double Building::c(const Time& t0, const Time& t1) const
    {
        double T0Ext = weather_->model.temperature(t0);
        double T1Ext = weather_->model.temperature(t1);
        double TExt = 0.5 * (T0Ext + T1Ext);

        return TExt + (copHeat_ * PHeat() - copCool_ * PCool()) / kb_;
    }

    void BuildingParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        YAML::Node subNd;

        assertFieldPresent(nd, "id");
        std::string id = parser.expand<std::string>(nd["id"]);

        assertFieldPresent(nd, "sim_network_id");
        std::string simNetworkId = parser.expand<std::string>(nd["sim_network_id"]);

        assertFieldPresent(nd, "bus_id");
        const std::string busId = parser.expand<std::string>(nd["bus_id"]);

        auto simNetw = sim.simComponent<SimNetwork>(simNetworkId);
        auto zip = simNetw->network().addZip(std::make_shared<Zip>(id, Phase::BAL), busId);

        auto build = sim.newSimComponent<Building>(id, zip);

        subNd = nd["dt"];
        if (subNd) build->set_dt(parser.expand<Time>(subNd));

        subNd = nd["kb"];
        if (subNd) build->set_kb(parser.expand<double>(subNd));

        subNd = nd["Cb"];
        if (subNd) build->setCb(parser.expand<double>(subNd));

        subNd = nd["Tb_init"];
        if (subNd) build->setTbInit(parser.expand<double>(subNd));

        subNd = nd["COP_cool"];
        if (subNd) build->setCopCool(parser.expand<double>(subNd));

        subNd = nd["COP_heat"];
        if (subNd) build->setCopHeat(parser.expand<double>(subNd));

        subNd = nd["max_P_cool"];
        if (subNd) build->setMaxPCool(parser.expand<double>(subNd));
        
        subNd = nd["max_P_heat"];
        if (subNd) build->setMaxPHeat(parser.expand<double>(subNd));

        subNd = nd["req_P_cool"];
        if (subNd) build->setReqPCool(parser.expand<double>(subNd));
        
        subNd = nd["req_P_heat"];
        if (subNd) build->setReqPHeat(parser.expand<double>(subNd));

        const auto& PThIntNd = nd["internal_heat_power"];
        if (PThIntNd)
        {
            std::string id = parser.expand<std::string>(PThIntNd);
            auto series = sim.timeSeries()[id].as<TimeSeries<Time, double>>();
            sgtAssert(series != nullptr, "Parsing building: couldn't find time series " << id << ".");
            build->setPThIntSeries(series);
        }

        const auto& weatherNd = nd["weather"];
        if (weatherNd)
        {
            std::string weatherStr = parser.expand<std::string>(weatherNd);
            auto weather = sim.simComponent<Weather>(weatherStr);
            sgtAssert(weather != nullptr, "Parsing simple_building: couldn't find weather " << weatherStr << ".");
            build->setWeather(weather);
        }
    }
}
