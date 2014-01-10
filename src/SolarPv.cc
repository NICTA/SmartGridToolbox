#include <SmartGridToolbox/SolarPv.h>
#include <SmartGridToolbox/InverterBase.h>
#include <SmartGridToolbox/Model.h>
#include <SmartGridToolbox/Weather.h>

namespace SmartGridToolbox
{
   SolarPv::SolarPv(const std::string & name) :
      DcPowerSourceBase(name),
      weather_(nullptr),
      efficiency_(1.0),
      planeNormal_({0.0, 0.0}),
      planeArea_(0.0)
   {}

   double SolarPv::PDc() const
   {
      return weather_->solarPower(planeNormal_, planeArea_) * efficiency_;
   }

   void SolarPv::setWeather(Weather & weather)
   {
      weather_ = &weather;
      dependsOn(weather);
      weather.didUpdate().addAction([this](){needsUpdate().trigger();}, "Trigger SolarPv " + name() + " needs update");
   }
};
