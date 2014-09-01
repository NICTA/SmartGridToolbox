#include "BatteryParser.h"

#include "InverterBase.h"
#include "Battery.h"

namespace SmartGridToolbox
{
   void BatteryParser::parse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "Battery : parse." << std::endl);

      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "inverter");

      string name = state.expandName(nd["name"].as<std::string>());
      Battery& batt = mod.newComponent<Battery>(name);

      auto nd_dt = nd["dt"];
      if (nd_dt) batt.set_dt(nd_dt.as<Time>());

      auto ndInitCharge = nd["init_charge"];
      if (ndInitCharge) batt.setInitCharge(ndInitCharge.as<double>());

      auto ndMaxCharge = nd["max_charge"];
      if (ndMaxCharge) batt.setMaxCharge(ndMaxCharge.as<double>());

      auto ndMaxChargePower = nd["max_charge_power"];
      if (ndMaxChargePower) batt.setMaxChargePower(ndMaxChargePower.as<double>());

      auto ndMaxDischargePower = nd["max_discharge_power"];
      if (ndMaxDischargePower) batt.setMaxDischargePower(ndMaxDischargePower.as<double>());

      auto ndChargeEfficiency = nd["charge_efficiency"];
      if (ndChargeEfficiency) batt.setChargeEfficiency(ndChargeEfficiency.as<double>());

      auto ndDischargeEfficiency = nd["discharge_efficiency"];
      if (ndDischargeEfficiency) batt.setDischargeEfficiency(ndDischargeEfficiency.as<double>());

      auto ndRequestedPower = nd["requested_power"];
      if (ndRequestedPower) batt.setRequestedPower(ndRequestedPower.as<double>());
   }

   void BatteryParser::postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "Battery : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      Battery& batt = *mod.component<Battery>(name);

      const std::string inverterStr = state.expandName(nd["inverter"].as<std::string>());
      InverterBase* inverter = mod.component<InverterBase>(inverterStr);
      if (inverter != nullptr)
      {
         inverter->addDcPowerSource(batt);
      }
      else
      {
         error() << "For component " << name << ", inverter " << inverterStr
                 << " was not found in the model." << std::endl;
         abort();
      }
   }
}
