#include "SolarPvParserPlugin.h"

#include "Inverter.h"
#include "Simulation.h"
#include "SolarPv.h"
#include "Weather.h"

namespace Sgt
{
   void SolarPvParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "weather");
      assertFieldPresent(nd, "inverter");
      assertFieldPresent(nd, "area_m2");
      assertFieldPresent(nd, "zenith_degrees");
      assertFieldPresent(nd, "azimuth_degrees");

      string id = parser.expand<std::string>(nd["id"]);
      auto spv = sim.newSimComponent<SolarPv>(id);

      if (nd["efficiency"])
      {
         spv->setEfficiency(parser.expand<double>(nd["efficiency"]));
      }
      else
      {
         spv->setEfficiency(1.0);
      }
      spv->setPlaneArea(parser.expand<double>(nd["area_m2"]));
      double zen = parser.expand<double>(nd["zenith_degrees"]) * pi / 180;
      double azi = parser.expand<double>(nd["azimuth_degrees"]) * pi / 180;
      spv->setPlaneNormal({zen, azi});

      const std::string weatherStr = parser.expand<std::string>(nd["weather"]);
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

      const std::string inverterStr = parser.expand<std::string>(nd["inverter"]);
      auto inverter = sim.simComponent<Inverter>(inverterStr);
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
