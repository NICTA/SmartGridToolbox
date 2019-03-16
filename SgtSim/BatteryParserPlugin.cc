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

#include "Battery.h"
#include "BatteryParserPlugin.h"
#include "Inverter.h"
#include "Simulation.h"

namespace Sgt
{
    void BatteryParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "inverter_id");

        std::string id = parser.expand<std::string>(nd["id"]);
        auto batt = sim.newSimComponent<Battery>(id);

        auto nd_dt = nd["dt"];
        if (nd_dt) batt->setDt(parser.expand<Time>(nd_dt));

        auto ndInitCharge = nd["init_soc"];
        if (ndInitCharge) batt->setInitSoc(parser.expand<double>(ndInitCharge));

        auto ndMaxCharge = nd["max_soc"];
        if (ndMaxCharge) batt->setMaxSoc(parser.expand<double>(ndMaxCharge));

        auto ndMaxChargePower = nd["max_charge_power"];
        if (ndMaxChargePower) batt->setMaxChargePower(parser.expand<double>(ndMaxChargePower));

        auto ndMaxDischargePower = nd["max_discharge_power"];
        if (ndMaxDischargePower) batt->setMaxDischargePower(parser.expand<double>(ndMaxDischargePower));

        auto ndChargeEfficiency = nd["charge_efficiency"];
        if (ndChargeEfficiency) batt->setChargeEfficiency(parser.expand<double>(ndChargeEfficiency));

        auto ndDischargeEfficiency = nd["discharge_efficiency"];
        if (ndDischargeEfficiency) batt->setDischargeEfficiency(parser.expand<double>(ndDischargeEfficiency));

        auto ndRequestedPower = nd["requested_power"];
        if (ndRequestedPower) batt->setRequestedPower(parser.expand<double>(ndRequestedPower));

        const std::string inverterStr = parser.expand<std::string>(nd["inverter_id"]);
        auto inverter = sim.simComponent<InverterAbc>(inverterStr);
        sgtAssert(inverter != nullptr,
                "For component " << id << ", inverter " << inverterStr << " was not found in the simulation.");
        inverter->addDcPowerSource(batt);
    }
}
