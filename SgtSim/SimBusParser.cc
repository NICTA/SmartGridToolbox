#include "SimBusParser.h"

#include "SimBus.h"
#include "SimNetwork.h"

#include <SgtCore/Common.h>

namespace SmartGridToolbox
{
   void SimBusParser::parse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "SimBus : parse." << std::endl);

      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "type");
      assertFieldPresent(nd, "phases");
      assertFieldPresent(nd, "nominal_voltage");

      string name = state.expandName(nd["name"].as<std::string>());
      BusType type = nd["type"].as<BusType>();
      Phases phases = nd["phases"].as<Phases>();
      int nPhase = phases.size();

      auto ndNominal = nd["nominal_voltage"];
      ublas::vector<Complex> nominalV = ndNominal ? ndNominal.as<ublas::vector<Complex>>()
                                                  : ublas::vector<Complex>(nPhase, czero);

      SimBus& bus = mod.newComponent<SimBus>(name, type, phases, nominalV);

      auto ndPg = nd["P_gen_setpoint"];
      if (ndPg) bus.setPgSetpoint(ndPg.as<ublas::vector<double>>());

      auto ndVMag = nd["V_mag_setpoint"];
      if (ndVMag) bus.setVMagSetpoint(ndVMag.as<ublas::vector<double>>());

      auto ndVAng = nd["V_ang_setpoint_deg"];
      if (ndVAng) bus.setVAngSetpoint(ndVAng.as<ublas::vector<double>>() * pi/180.0);
   }

   void SimBusParser::postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "SimBus : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      SimBus* comp = mod.component<SimBus>(name);

      const std::string networkStr = state.expandName(nd["network"].as<std::string>());
      SimNetwork* networkComp = mod.component<SimNetwork>(networkStr);
      if (networkComp != nullptr)
      {
         networkComp->addBus(*comp);
      }
      else
      {
         error() << "For component " << name << ", network " << networkStr
                 << " was not found in the model." << std::endl;
         abort();
      }
   }
}
