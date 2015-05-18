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

#include "SolarPvParserPlugin.h"

#include "Inverter.h"
#include "Simulation.h"
#include "SolarPv.h"
#include "Weather.h"

namespace Sgt
{
    void SolarPvParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "weather");
        assertFieldPresent(nd, "inverter");
        assertFieldPresent(nd, "area_m2");
        assertFieldPresent(nd, "zenith_degrees");
        assertFieldPresent(nd, "azimuth_degrees");

        string id = parser.expand<std::string>(nd["id"]);
        auto spv = sim.newSimComponent<SolarPv>(id);

        if (nd["efficiency"])
        {
            spv->setEfficiency(parser.expand<double>(nd["efficiency"]));
        }
        else
        {
            spv->setEfficiency(1.0);
        }
        spv->setPlaneArea(parser.expand<double>(nd["area_m2"]));
        double zen = parser.expand<double>(nd["zenith_degrees"]) * pi / 180;
        double azi = parser.expand<double>(nd["azimuth_degrees"]) * pi / 180;
        spv->setPlaneNormal({zen, azi});

        const std::string weatherStr = parser.expand<std::string>(nd["weather"]);
        auto weather = sim.simComponent<Weather>(weatherStr);
        if (weather != nullptr)
        {
            spv->setWeather(weather);
        }
        else
        {
            Log().error() << "For component " << id << ", weather " << weatherStr
                          << " was not found in the model." << std::endl;
            error();
        }

        const std::string inverterStr = parser.expand<std::string>(nd["inverter"]);
        auto inverter = sim.simComponent<InverterAbc>(inverterStr);
        if (inverter != nullptr)
        {
            inverter->addDcPowerSource(spv);
        }
        else
        {
            Log().error() << "For component " << id << ", inverter " << inverterStr
                          << " was not found in the model." << std::endl;
            error();
        }
    }
}
