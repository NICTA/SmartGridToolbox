#include "Battery.h"
#include "BatteryParserPlugin.h"
#include "Inverter.h"
#include "Simulation.h"

namespace SmartGridToolbox
{
   void BatteryParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "inverter");

      string id = parser.expand<std::string>(nd["id"]);
      auto batt = sim.newSimComponent<Battery>(id);

      auto nd_dt = nd["dt"];
      if (nd_dt) batt->set_dt(nd_dt.as<Time>());

      auto ndInitCharge = nd["init_charge"];
      if (ndInitCharge) batt->setInitCharge(ndInitCharge.as<double>());

      auto ndMaxCharge = nd["max_charge"];
      if (ndMaxCharge) batt->setMaxCharge(ndMaxCharge.as<double>());

      auto ndMaxChargePower = nd["max_charge_power"];
      if (ndMaxChargePower) batt->setMaxChargePower(ndMaxChargePower.as<double>());

      auto ndMaxDischargePower = nd["max_discharge_power"];
      if (ndMaxDischargePower) batt->setMaxDischargePower(ndMaxDischargePower.as<double>());

      auto ndChargeEfficiency = nd["charge_efficiency"];
      if (ndChargeEfficiency) batt->setChargeEfficiency(ndChargeEfficiency.as<double>());

      auto ndDischargeEfficiency = nd["discharge_efficiency"];
      if (ndDischargeEfficiency) batt->setDischargeEfficiency(ndDischargeEfficiency.as<double>());

      auto ndRequestedPower = nd["requested_power"];
      if (ndRequestedPower) batt->setRequestedPower(ndRequestedPower.as<double>());

      const std::string inverterStr = parser.expand<std::string>(nd["inverter"]);
      auto inverter = sim.simComponent<InverterAbc>(inverterStr);
      if (inverter != nullptr)
      {
         inverter->addDcPowerSource(batt);
      }
      else
      {
         Log().fatal() << "For component " << id << ", inverter " << inverterStr
                 << " was not found in the model." << std::endl;
      }
   }
}
