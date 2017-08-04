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
        assertFieldPresent(nd, "zenith_degrees");
        assertFieldPresent(nd, "azimuth_degrees");
        assertFieldPresent(nd, "n_panels");
        assertFieldPresent(nd, "panel_area_m2");
        assertFieldPresent(nd, "irradiance_ref_W_per_m2");
        assertFieldPresent(nd, "T_cell_ref_C");
        assertFieldPresent(nd, "P_max_ref_W");
        assertFieldPresent(nd, "temp_coeff_P_max_per_C");
        assertFieldPresent(nd, "NOCT_C");

        std::string id = parser.expand<std::string>(nd["id"]);
        auto spv = sim.newSimComponent<SolarPv>(id);

        const std::string weatherStr = parser.expand<std::string>(nd["weather"]);
        auto weather = sim.simComponent<Weather>(weatherStr);
        sgtAssert(weather != nullptr, 
                "For component " << id << ", weather " << weatherStr << " was not found in the simulation.");
        spv->setWeather(weather);
        
        const std::string inverterStr = parser.expand<std::string>(nd["inverter"]);
        auto inverter = sim.simComponent<InverterAbc>(inverterStr);
        sgtAssert(inverter != nullptr,
            "For component " << id << ", inverter " << inverterStr << " was not found in the simulation.");
        inverter->addDcPowerSource(spv);

        spv->setNPanels(parser.expand<int>(nd["n_panels"]));
        double zen = parser.expand<double>(nd["zenith_degrees"]) * pi / 180;
        double azi = parser.expand<double>(nd["azimuth_degrees"]) * pi / 180;
        spv->setPlaneNormal({zen, azi});
        spv->setPanelArea(parser.expand<double>(nd["panel_area_m2"]));
        spv->setPhiRef(nd["irradiance_ref_W_per_m2"].as<double>());
        spv->setTRef(nd["T_cell_ref_C"].as<double>() + 273.0);
        spv->setPMaxRef(nd["P_max_ref_W"].as<double>());
        spv->setTempCoeffPMax(nd["temp_coeff_P_max_per_C"].as<double>());
        spv->setNOCT(nd["NOCT_C"].as<double>() + 273.0);
    }
}
