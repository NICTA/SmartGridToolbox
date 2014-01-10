#include "BusParser.h"

#include <SmartGridToolbox/Bus.h>
#include <SmartGridToolbox/Common.h>
#include <SmartGridToolbox/Network.h>

namespace SmartGridToolbox
{
   void BusParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "Bus : parse." << std::endl);

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
      
      Bus & bus = mod.newComponent<Bus>(name, type, phases, nominalV);

      auto ndPg = nd["P_gen_setpoint"];
      if (ndPg) bus.setPgSetpoint(ndPg.as<ublas::vector<double>>());

      auto ndPgMin = nd["P_gen_min_setpoint"];
      if (ndPgMin) bus.setPgMinSetpoint(ndPgMin.as<ublas::vector<double>>());
      
      auto ndPgMax = nd["P_gen_max_setpoint"];
      if (ndPgMax) bus.setPgMaxSetpoint(ndPgMax.as<ublas::vector<double>>());

      auto ndQg = nd["Q_gen_setpoint"];
      if (ndQg) bus.setQgSetpoint(ndQg.as<ublas::vector<double>>());

      auto ndQgMin = nd["Q_gen_min_setpoint"];
      if (ndQgMin) bus.setQgMinSetpoint(ndQgMin.as<ublas::vector<double>>());
      
      auto ndQgMax = nd["Q_gen_max_setpoint"];
      if (ndQgMax) bus.setQgMaxSetpoint(ndQgMax.as<ublas::vector<double>>());

      auto ndVMag = nd["V_mag_setpoint"];
      if (ndVMag) bus.setVMagSetpoint(ndVMag.as<ublas::vector<double>>());

      auto ndVMagMin = nd["V_mag_min_setpoint"];
      if (ndVMagMin) bus.setVMagMinSetpoint(ndVMagMin.as<ublas::vector<double>>());
      
      auto ndVMagMax = nd["V_mag_max_setpoint"];
      if (ndVMagMax) bus.setVMagMaxSetpoint(ndVMagMax.as<ublas::vector<double>>());

      auto ndVAng = nd["V_ang_setpoint_rad"];
      if (ndVAng) bus.setVAngSetpoint(ndVAng.as<ublas::vector<double>>());

      auto ndVAngMin = nd["V_ang_min_setpoint_rad"];
      if (ndVAngMin) bus.setVAngMinSetpoint(ndVAngMin.as<ublas::vector<double>>());
      
      auto ndVAngMax = nd["V_ang_max_setpoint_rad"];
      if (ndVAngMax) bus.setVAngMaxSetpoint(ndVAngMax.as<ublas::vector<double>>());
   }

   void BusParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "Bus : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      Bus * comp = mod.component<Bus>(name);

      const std::string networkStr = state.expandName(nd["network"].as<std::string>());
      Network * networkComp = mod.component<Network>(networkStr);
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
