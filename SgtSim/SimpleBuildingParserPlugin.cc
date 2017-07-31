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

#include "SimpleBuildingParserPlugin.h"

#include "SimpleBuilding.h"
#include "SimNetwork.h"
#include "Simulation.h"
#include "Weather.h"

namespace Sgt
{
    void SimpleBuildingParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "sim_network_id");
        assertFieldPresent(nd, "bus_id");

        std::string id = parser.expand<std::string>(nd["id"]);

        std::string networkId = parser.expand<std::string>(nd["sim_network_id"]);
        const std::string busId = parser.expand<std::string>(nd["bus_id"]);

        // TODO: Would be better to force zip to be created explicitly, or
        // used from elsewhere.
        auto& simNetwork = *sim.simComponent<SimNetwork>(networkId);
        auto zip = simNetwork.network().addZip(std::make_shared<Zip>(id, Phase::BAL), busId);

        auto build = sim.newSimComponent<SimpleBuilding>(id, zip, simNetwork);

        auto nd_dt = nd["dt"];
        if (nd_dt) build->set_dt(parser.expand<Time>(nd_dt));

        auto nd_kb = nd["kb"];
        if (nd_kb) build->set_kb(parser.expand<double>(nd_kb));

        auto ndCb = nd["Cb"];
        if (ndCb) build->setCb(parser.expand<double>(ndCb));

        auto ndTbInit = nd["Tb_init"];
        if (ndTbInit) build->setTbInit(parser.expand<double>(ndTbInit));

        auto nd_kh = nd["kh"];
        if (nd_kh) build->set_kh(parser.expand<double>(nd_kh));

        auto ndCopCool = nd["COP_cool"];
        if (ndCopCool) build->setCopCool(parser.expand<double>(ndCopCool));

        auto ndCopHeat = nd["COP_heat"];
        if (ndCopHeat) build->setCopHeat(parser.expand<double>(ndCopHeat));

        auto ndPMax = nd["P_max"];
        if (ndPMax) build->setPMax(parser.expand<double>(ndPMax));

        auto ndTs = nd["Ts"];
        if (ndTs) build->setTs(parser.expand<double>(ndTs));

        const auto& dQgNd = nd["internal_heat_power"];
        if (dQgNd)
        {
            std::string id = parser.expand<std::string>(dQgNd);
            auto series = sim.timeSeries()[id].as<TimeSeries<Time, double>>();
            sgtAssert(series != nullptr, "Parsing simple_building: couldn't find time series " << id << ".");
            build->set_dQgSeries(series);
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
