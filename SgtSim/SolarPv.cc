// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "SolarPv.h"

#include "Inverter.h"
#include "Weather.h"

namespace Sgt
{
    SolarPv::SolarPv(const std::string& id) :
        Component(id)
    {}

    void SolarPv::setWeather(std::shared_ptr<Weather> weather)
    {
        weather_ = weather;
        dependsOn(weather);
        weather->didUpdate().addAction([this]() {dcPowerChanged().trigger();},
        std::string("Trigger ") + sComponentType() + " " + id() + " DC power changed");
    }

    double SolarPv::PDc(const Time& t) const
    {
        // Note: convert from SI to MW.
        // Also need to multiply by number of panels.
        return 1e-6 * PMaxRef_ * nPanels_ * 
            (solarIrradiance(weather_->model.irradiance(t), planeNormal_) / phiRef_) * 
            (1.0 + tempCoeffPMax_ * (TCell(t) - TRef_));
    }

    double SolarPv::TCell(const Time& t) const
    {
        // See http://www.pveducation.org/pvcdrom/modules/nominal-operating-cell-temperature
        // Some conversion C <-> K, W/m^2 <-> mW/cm^2.
        return weather_->model.temperature(t) + ((NOCT_ - 293.0) / 800.0) * 
            solarIrradiance(weather_->model.irradiance(t), planeNormal_) + 273.0;
    }
};
