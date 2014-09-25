#include "InverterParserPlugin.h"

#include "Inverter.h"
#include "SimNetwork.h"
#include "Simulation.h"

namespace SmartGridToolbox
{
   void InverterParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases");
      assertFieldPresent(nd, "network_id");
      assertFieldPresent(nd, "bus_id");

      string id = parser.expand<std::string>(nd["id"]);
      Phases phases = nd["phases"].as<Phases>();

      auto inverter = sim.newSimComponent<Inverter>(id, phases);
         
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

      const std::string networkId = parser.expand<std::string>(nd["network_id"]);
      const std::string busId = parser.expand<std::string>(nd["bus_id"]);

      auto network = sim.simComponent<SimNetwork>(networkId);
      network->addZip(inverter, busId);
   }
}
