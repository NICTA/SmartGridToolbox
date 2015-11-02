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

#include "Building.h"

#include <SgtSim/Simulation.h>

namespace Sgt
{
    void Building::initializeState()
    {
        Tb_ = TbInit_;
    }

    void Building::updateState(Time t)
    {
        double dt = lastUpdated() == posix_time::neg_infin ? 0 : dSeconds(t - lastUpdated());

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
    }

    void Building::setWeather(std::shared_ptr<Weather> weather)
    {
        weather_ = weather;
        dependsOn(weather);
        needsUpdate().trigger();
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
        auto build = sim.newSimComponent<Building>(id);

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
            auto series = sim.timeSeries<TimeSeries<Time, double>>(id);
            sgtAssert(series != nullptr, "Parsing building: couldn't find time series " << id << ".");
            build->setPThIntSeries(series);
        }

        assertFieldPresent(nd, "sim_network_id");
        std::string networkId = parser.expand<std::string>(nd["sim_network_id"]);

        assertFieldPresent(nd, "bus_id");
        const std::string busId = parser.expand<std::string>(nd["bus_id"]);

        auto netw = sim.simComponent<SimNetwork>(networkId);
        netw->network()->addZip(build->zip(), busId);
        build->linkToSimNetwork(*netw);

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
