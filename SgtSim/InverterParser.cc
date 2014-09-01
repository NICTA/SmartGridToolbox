#include "InverterParser.h"

#include "Inverter.h"
#include "SimNetwork.h"
#include "Simulation.h"

namespace SmartGridToolbox
{
   void InverterParser::parse(const YAML::Node& nd, Simulation& into) const
   {
      SGT_DEBUG(debug() << "Inverter : parse." << std::endl);

      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "bus");

      string id = nd["id"].as<std::string>();
      Phases phases = nd["phases"].as<Phases>();
      const std::string networkStr = nd["network"].as<std::string>();
      const std::string busStr = nd["bus"].as<std::string>();

      auto inverter = into.newSimComponent<Inverter>(id, phases);

      if (nd["efficiency"])
      {
         inverter->setEfficiency(nd["efficiency"].as<double>());
      }

      if (nd["max_S_mag_per_phase"])
      {
         inverter->setMaxSMagPerPhase(nd["max_S_mag_per_phase"].as<double>());
      }

      if (nd["min_power_factor"])
      {
         inverter->setMinPowerFactor(nd["min_power_factor"].as<double>());
      }

      if (nd["requested_Q_per_phase"])
      {
         inverter->setRequestedQPerPhase(nd["requested_Q_per_phase"].as<double>());
      }

      auto network = into.simComponent<SimNetwork>(networkStr);
      network->addZip(inverter,  
   }
}
