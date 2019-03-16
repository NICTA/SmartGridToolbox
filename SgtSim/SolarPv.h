// Copyright 2015-2016 National ICT Australia Limited (NICTA)
// Copyright 2016-2019 The Australian National University
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

#ifndef SOLAR_PV_DOT_H
#define SOLAR_PV_DOT_H

#include <SgtSim/DcPowerSource.h>
#include <SgtSim/Sun.h>

namespace Sgt
{
    class Weather;

    class SolarPv : public DcPowerSourceAbc
    {

        /// @name Static member functions:
        /// @{

        public:

        static const std::string& sComponentType()
        {
            static std::string result("solar_pv");
            return result;
        }

        /// @}

        /// @name Lifecycle:
        /// @{

        SolarPv(const std::string& id);

        /// @}

        /// @name Component virtual overridden member functions.
        /// @{

        virtual const std::string& componentType() const override
        {
            return sComponentType();
        }

        /// @}

        /// @name My public member functions.
        /// @{

        /// @brief Set the weather component.
        /// @param weather The weather component
        void setWeather(const ConstSimComponentPtr<Weather>& weather);

        /// @brief Set the unit normal to the plane of the solar panels.
        /// @param planeNormal The SphericalAngles object decribing the plane normal.
        void setPlaneNormal(SphericalAngles planeNormal) {planeNormal_ = planeNormal; needsUpdate().trigger();}

        /// @brief Set the number of panels.
        /// @param nPanels The number of panels.
        void setNPanels(int nPanels) {nPanels_ = nPanels; needsUpdate().trigger();}

        /// @brief Set the area of each panel.
        /// @param panelArea The area in m^2
        void setPanelArea(double panelArea) {panelArea_ = panelArea; needsUpdate().trigger();}

        /// @brief Set the reference irradiance.
        /// @param phiRef Irradiance in W/m^2, should be given in datasheet.
        /// The reference irradiance is usually taken to be 1000 W/m^2 (1 sun).
        void setPhiRef(double phiRef) {phiRef_ = phiRef; needsUpdate().trigger();}

        /// @brief Set the reference temperature.
        /// @param TRef Temperature in K of cells under reference conditions, should be given in datasheet.
        /// The reference temperature is usually taken to be 25 C, although the nominal operating cell temperature
        /// (NOCT) is usually higher than this.
        /// See <http://www.pveducation.org/pvcdrom/modules/nominal-operating-cell-temperature>.
        void setTRef(double TRef) {TRef_ = TRef; needsUpdate().trigger();}

        /// @brief Set the maximum power (power at the MPP) in W under reference conditions.
        /// @param PMaxRef Maximum power in W, should be given in datasheet.
        void setPMaxRef(double PMaxRef) {PMaxRef_ = PMaxRef; needsUpdate().trigger();}

        /// @brief Set the temperature coefficient of maximum power relative to TRef.
        /// @param tempCoeffPMax Temperature coefficient of maximum power, K^-1. Should be given in datasheet.
        /// Quantifies linear effect on maximum power of deviations from reference temperature.
        /// Negative = fractional reduction in max. power per degree ABOVE reference temp.
        void setTempCoeffPMax(double tempCoeffPMax) {tempCoeffPMax_ = tempCoeffPMax; needsUpdate().trigger();}

        /// @brief Set the NOCT (nominal operating cell temperature)
        /// @param NOCT Temperature in K.
        void setNOCT(double NOCT) {NOCT_ = NOCT; needsUpdate().trigger();}

        /// @brief Power output in MW, assuming efficient maximum power point tracking (MPPT).
        virtual double requestedPDc(const Time& t) const;

        /// @brief Power output in MW, assuming efficient maximum power point tracking (MPPT).
        virtual double requestedPDc() const override
        {
            return requestedPDc_;
        }

        /// @}

        /// @name Some utility calculated quantities:
        /// @{

        /// @brief Temperature of the cell, in K.
        /// See http://www.pveducation.org/pvcdrom/modules/nominal-operating-cell-temperature
        double TCell(const Time& t) const;

        /// @brief Temperature of the cell, in K.
        /// See http://www.pveducation.org/pvcdrom/modules/nominal-operating-cell-temperature
        double TCell() const {return TCell(lastUpdated());}

        /// @}

        /// @name SimComponent virtual overridden member functions.
        /// @{

        protected:

        virtual void updateState(const Time& t) override;

        /// @}

        private:

        ConstSimComponentPtr<Weather> weather_;

        // Parameters:
        SphericalAngles planeNormal_;
        int nPanels_;
        double panelArea_;
        double phiRef_{1000.0}; // Reference irradiance, usually 1000 W/m^2
        double TRef_{298.0}; // Reference temperature (K), usually 25 C 
        double PMaxRef_; // Maximum power of a single panel at reference conditions.
        double tempCoeffPMax_; // Temperature coefficient of maximum power, relative to TRef.
        double NOCT_; // Nominal operating cell temperature, should be given in datasheet.

        // State:
        double requestedPDc_{0.0};
    };
}

#endif // SOLAR_PV_DOT_H
