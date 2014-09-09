#include "SolarPvParser.h"

#include "Inverter.h"
#include "Simulation.h"
#include "SolarPv.h"
#include "Weather.h"

namespace SmartGridToolbox
{
   void SolarPvParser::parse(const YAML::Node& nd, Simulation& into) const
   {
      SGT_DEBUG(debug() << "SolarPv : parse." << std::endl);
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "weather");
      assertFieldPresent(nd, "inverter");
      assertFieldPresent(nd, "area_m2");
      assertFieldPresent(nd, "zenith_degrees");
      assertFieldPresent(nd, "azimuth_degrees");

      string id = nd["id"].as<std::string>();
      auto spv = into.newSimComponent<SolarPv>(id);

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
      auto weather = into.simComponent<Weather>(weatherStr);
      if (weather != nullptr)
      {
         spv->setWeather(weather);
      }
      else
      {
         error() << "For component " << id << ", weather " << weatherStr
                 << " was not found in the model." << std::endl;
         abort();
      }

      const std::string inverterStr = nd["inverter"].as<std::string>();
      auto inverter = into.simComponent<InverterAbc>(inverterStr);
      if (inverter != nullptr)
      {
         inverter->addDcPowerSource(spv);
      }
      else
      {
         error() << "For component " << id << ", inverter " << inverterStr
                 << " was not found in the model." << std::endl;
         abort();
      }
   }
}
