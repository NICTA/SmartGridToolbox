#include "SolarPvParser.h"

#include "Inverter.h"
#include "Simulation.h"
#include "SolarPv.h"
#include "Weather.h"

namespace SmartGridToolbox
{
   void SolarPvParser::parse(const YAML::Node& nd, Simulation& sim, const ParserState& state) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "weather");
      assertFieldPresent(nd, "inverter");
      assertFieldPresent(nd, "area_m2");
      assertFieldPresent(nd, "zenith_degrees");
      assertFieldPresent(nd, "azimuth_degrees");

      string id = state.expandName(nd["id"].as<std::string>());
      auto spv = sim.newSimComponent<SolarPv>(id);

      if (nd["efficiency"])
      {
         spv->setEfficiency(nd["efficiency"].as<double>());
      }
      else
      {
         spv->setEfficiency(1.0);
      }
      spv->setPlaneArea(nd["area_m2"].as<double>());
      double zen = nd["zenith_degrees"].as<double>() * pi / 180;
      double azi = nd["azimuth_degrees"].as<double>() * pi / 180;
      spv->setPlaneNormal({zen, azi});

      const std::string weatherStr = nd["weather"].as<std::string>();
      auto weather = sim.simComponent<Weather>(weatherStr);
      if (weather != nullptr)
      {
         spv->setWeather(weather);
      }
      else
      {
         Log().fatal() << "For component " << id << ", weather " << weatherStr
                 << " was not found in the model." << std::endl;
      }

      const std::string inverterStr = nd["inverter"].as<std::string>();
      auto inverter = sim.simComponent<SimInverter>(inverterStr);
      if (inverter != nullptr)
      {
         inverter->addDcPowerSource(spv);
      }
      else
      {
         Log().fatal() << "For component " << id << ", inverter " << inverterStr
                 << " was not found in the model." << std::endl;
      }
   }
}
