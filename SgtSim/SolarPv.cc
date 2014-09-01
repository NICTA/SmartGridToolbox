#include "SolarPv.h"

#include "InverterBase.h"
#include "Model.h"
#include "Weather.h"

namespace SmartGridToolbox
{
   SolarPv::SolarPv(const std::string& id) :
      DcPowerSourceBase(id),
      weather_(nullptr),
      efficiency_(1.0),
      planeNormal_({0.0, 0.0}),
      planeArea_(0.0)
   {}

   void SolarPv::setWeather(Weather& weather)
   {
      weather_ = &weather;
      dependsOn(weather);
      weather.didUpdate().addAction([this](){needsUpdate().trigger();}, "Trigger SolarPv " + id() + " needs update");
   }

   double SolarPv::PDc() const
   {
      return weather_->solarPower(planeNormal_, planeArea_) * efficiency_;
   }
};
