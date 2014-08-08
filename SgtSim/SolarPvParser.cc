#include "SolarPvParser.h"

#include <SgtSim/InverterBase.h>
#include <SgtSim/SolarPv.h>
#include <SgtSim/Weather.h>

namespace SmartGridToolbox
{
   void SolarPvParser::parse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "SolarPv : parse." << std::endl);
      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "weather");
      assertFieldPresent(nd, "inverter");
      assertFieldPresent(nd, "area_m2");
      assertFieldPresent(nd, "zenith_degrees");
      assertFieldPresent(nd, "azimuth_degrees");

      string name = state.expandName(nd["name"].as<std::string>());
      SolarPv& comp = mod.newComponent<SolarPv>(name);

      if (nd["efficiency"])
      {
         comp.setEfficiency(nd["efficiency"].as<double>());
      }
      else
      {
         comp.setEfficiency(1.0);
      }
      comp.setPlaneArea(nd["area_m2"].as<double>());
      double zen = nd["zenith_degrees"].as<double>()*pi/180;
      double azi = nd["azimuth_degrees"].as<double>()*pi/180;
      comp.setPlaneNormal({zen, azi});
   }

   void SolarPvParser::postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "SolarPv : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      SolarPv& comp = *mod.component<SolarPv>(name);

      const std::string weatherStr = state.expandName(nd["weather"].as<std::string>());
      Weather* weather = mod.component<Weather>(weatherStr);
      if (weather != nullptr)
      {
         comp.setWeather(*weather);
      }
      else
      {
         error() << "For component " << name << ", weather " << weatherStr
                 << " was not found in the model."
                 << std::endl;
         abort();
      }

      const std::string inverterStr = state.expandName(nd["inverter"].as<std::string>());
      InverterBase* inverter = mod.component<InverterBase>(inverterStr);
      if (inverter != nullptr)
      {
         inverter->addDcPowerSource(comp);
      }
      else
      {
         error() << "For component " << name << ", inverter " << inverterStr
                 << " was not found in the model." << std::endl;
         abort();
      }
   }
}
