#include <smartgridtoolbox/SolarPV.h>
#include <smartgridtoolbox/InverterBase.h>
#include <smartgridtoolbox/Model.h>
#include <smartgridtoolbox/Weather.h>

namespace SmartGridToolbox
{
   void SolarPVParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "SolarPV : parse." << std::endl);
      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "weather");
      assertFieldPresent(nd, "inverter");
      assertFieldPresent(nd, "area_m2");
      assertFieldPresent(nd, "zenith_degrees");
      assertFieldPresent(nd, "azimuth_degrees");

      string name = state.expandName(nd["name"].as<std::string>());
      SolarPV & comp = mod.newComponent<SolarPV>(name);

      if (nd["efficiency"])
      {
         comp.setEfficiency(nd["efficiency"].as<double>());
      }
      else
      {
         comp.setEfficiency(1.0);
      }
      comp.setPlaneArea(nd["area_m2"].as<double>());
      double zen = nd["zenith_degrees"].as<double>() * pi / 180;
      double azi = nd["azimuth_degrees"].as<double>() * pi / 180;
      comp.setPlaneNormal({zen, azi});
   }

   void SolarPVParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "SolarPV : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      SolarPV & comp = *mod.componentNamed<SolarPV>(name);

      const std::string weatherStr = state.expandName(nd["weather"].as<std::string>());
      Weather * weather = mod.componentNamed<Weather>(weatherStr);
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
      InverterBase * inverter = mod.componentNamed<InverterBase>(inverterStr);
      if (inverter != nullptr)
      {
         inverter->addDCPowerSource(comp);
      }
      else
      {
         error() << "For component " << name << ", inverter " << inverterStr
                 << " was not found in the model." << std::endl;
         abort();
      }
   }

   SolarPV::SolarPV(const std::string & name) :
      DCPowerSourceBase(name),
      weather_(nullptr),
      efficiency_(1.0),
      planeNormal_({0.0, 0.0}),
      planeArea_(0.0)
   {}

   double SolarPV::PDC() const
   {
      return weather_->solarPower(planeNormal_, planeArea_) * efficiency_;
   }

   void SolarPV::setWeather(Weather & weather)
   {
      weather_ = &weather;
      dependsOn(weather);
      weather.didUpdate().addAction([this](){needsUpdate().trigger();}, "Trigger SolarPV " + name() + " needs update");
   }
};
