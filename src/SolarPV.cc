#include "SolarPV.h"
#include "InverterBase.h"
#include "Model.h"
#include "Weather.h"

namespace SmartGridToolbox
{
   void SolarPVParser::parse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "SolarPV : parse." << std::endl);
      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "weather");
      assertFieldPresent(nd, "inverter");
      assertFieldPresent(nd, "area_m2");
      assertFieldPresent(nd, "zenith_degrees");
      assertFieldPresent(nd, "azimuth_degrees");

      SolarPV & comp = mod.newComponent<SolarPV>(nd["name"].as<std::string>());
      if (nd["efficiency"])
      {
         comp.setEfficiency(nd["efficiency"].as<double>());
      }
      else
      {
         comp.setEfficiency(1.0);
      }
      comp.setPlaneArea(nd["area_m2"].as<double>());
      comp.setPlaneNormal({nd["zenith_degrees"].as<double>(), nd["azimuth_degrees"].as<double>()});
   }

   void SolarPVParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "SolarPV : postParse." << std::endl);

      const std::string nameStr = nd["name"].as<std::string>();
      SolarPV & comp = *mod.componentNamed<SolarPV>(nameStr);

      const std::string weatherStr = nd["weather"].as<std::string>();
      Weather * weather = mod.componentNamed<Weather>(weatherStr);
      if (weather != nullptr)
      {
         comp.setWeather(*weather);
      }
      else
      {
         error() << "For component " << nameStr << ", weather " << weatherStr << " was not found in the model." 
                 << std::endl;
         abort();
      }

      const std::string inverterStr = nd["inverter"].as<std::string>();
      InverterBase * inverter = mod.componentNamed<InverterBase>(inverterStr);
      if (inverter != nullptr)
      {
         inverter->addDCPowerSource(comp);
      }
      else
      {
         error() << "For component " << nameStr << ", inverter " << inverterStr << " was not found in the model." 
                 << std::endl;
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
};
