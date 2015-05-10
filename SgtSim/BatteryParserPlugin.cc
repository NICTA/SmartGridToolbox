#include "Battery.h"
#include "BatteryParserPlugin.h"
#include "Inverter.h"
#include "Simulation.h"

namespace Sgt
{
    void BatteryParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "inverter");

        string id = parser.expand<std::string>(nd["id"]);
        auto batt = sim.newSimComponent<Battery>(id);

        auto nd_dt = nd["dt"];
        if (nd_dt) batt->set_dt(parser.expand<Time>(nd_dt));

        auto ndInitCharge = nd["init_charge"];
        if (ndInitCharge) batt->setInitCharge(parser.expand<double>(ndInitCharge));

        auto ndMaxCharge = nd["max_charge"];
        if (ndMaxCharge) batt->setMaxCharge(parser.expand<double>(ndMaxCharge));

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

        const std::string inverterStr = parser.expand<std::string>(nd["inverter"]);
        auto inverter = sim.simComponent<InverterAbc>(inverterStr);
        if (inverter != nullptr)
        {
            inverter->addDcPowerSource(batt);
        }
        else
        {
            Log().error() << "For component " << id << ", inverter " << inverterStr
                          << " was not found in the model." << std::endl;
            error();
        }
    }
}
