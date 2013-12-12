#include <SmartGridToolbox/SimpleBattery.h>
#include <iostream>

using namespace std;

namespace SmartGridToolbox
{
   void SimpleBatteryParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "SimpleBattery : parse." << std::endl);

      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "inverter");

      string name = state.expandName(nd["name"].as<std::string>());
      SimpleBattery & batt = mod.newComponent<SimpleBattery>(name);

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
      
      auto ndRequestedPower = nd["requestedPower"];
      if (ndRequestedPower) batt.setRequestedPower(ndRequestedPower.as<double>());
   }

   void SimpleBattery::updateState(Time t0, Time t1)
   {
      charge_ += internalPower() * dSeconds(t1 - t0);
      if (charge_ < 0.0) charge_ = 0.0;
   }
}
