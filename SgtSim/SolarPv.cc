#include "SolarPv.h"

#include "Inverter.h"
#include "Weather.h"

namespace Sgt
{
    SolarPv::SolarPv(const std::string& id) :
        DcPowerSourceAbc(id),
        weather_(nullptr),
        efficiency_(1.0),
        planeNormal_( {0.0, 0.0}),
                  planeArea_(0.0)
    {}

    void SolarPv::setWeather(std::shared_ptr<Weather> weather)
    {
        weather_ = weather;
        dependsOn(weather);
        weather->didUpdate().addAction([this]() {dcPowerChanged().trigger();},
        std::string("Trigger ") + sComponentType() + " " + id() + " DC power changed");
    }

    double SolarPv::PDc() const
    {
        // Note: convert from SI to MW.
        return 1e-6*weather_->solarPower(planeNormal_, planeArea_) * efficiency_;
    }
};
