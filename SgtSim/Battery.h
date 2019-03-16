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

#ifndef BATTERY_DOT_H
#define BATTERY_DOT_H

#include <SgtSim/DcPowerSource.h>

#include <SgtCore/Common.h>

#include<string>

namespace Sgt
{
    /// @brief Basic battery class, deriving from DCPowerSourceBase.
    ///
    /// Units: SmartGridToolbox system of units uses MW for power. For the Battery class, we shall therefore measure
    /// SOC in units of MWh.
    ///
    /// Sign convention: like other components, a positive power means injection into the grid. Therefore, charging
    /// is positive and discharging is negative.
    class Battery : public DcPowerSourceAbc
    {

        /// @name Lifecycle
        /// @{

        public:

        Battery(const std::string& id) :
            Component(id),
            dt_(minutes(5)),
            initSoc_(0.0),
            maxChargePower_(0.0),
            maxDischargePower_(0.0),
            chargeEfficiency_(0.0),
            dischargeEfficiency_(0.0),
            requestedPower_(0.0),
            soc_(0.0)
        {
            // Empty.
        }

        /// @}

        /// @name Component virtual overridden member functions.
        /// @{

        public:

        static const std::string& sComponentType()
        {
            static std::string result("battery");
            return result;
        }

        virtual const std::string& componentType() const override
        {
            return sComponentType();
        }

        // virtual json toJson() const override; // TODO

        /// @}

        /// @name Overridden member functions from SimComponent.
        /// @{

        public:

        virtual Time validUntil() const override
        {
            return lastUpdated() + dt_;
        }

        protected:

        virtual void initializeState() override
        {
            soc_ = initSoc_;
        }

        virtual void updateState(const Time& t) override;

        /// @}

        /// @name Overridden member functions from DcPowerSourceAbc.
        /// @{

        public:

        /// Positive = charging.
        virtual double requestedPDc() const override
        {
            return requestedPDc_;
        }

        /// @}

        /// @name Battery specific member functions.
        /// @{

        Time dt() const {return dt_;}
        void setDt(Time val) {dt_ = val;}
        // Doesn't affect state this timestep, no update needed.

        double initSoc() const {return initSoc_;}
        void setInitSoc(double val) {initSoc_ = val;}
        // Doesn't affect state this timestep, no update needed.

        double maxSoc() const {return maxSoc_;}
        void setMaxSoc(double val);

        double maxChargePower() const {return maxChargePower_;}
        void setMaxChargePower(double val);

        double maxDischargePower() const {return maxDischargePower_;}
        void setMaxDischargePower(double val);

        double chargeEfficiency() const {return chargeEfficiency_;}
        void setChargeEfficiency(double val);

        double dischargeEfficiency() const {return dischargeEfficiency_;}
        void setDischargeEfficiency(double val);

        double soc() const {return soc_;}

        double requestedPower() const {return requestedPower_;} // +ve = injection into grid.
        void setRequestedPower(double val);

        double internalPower() const;

        /// @}

        private:

        double calcRequestedPDc() const;

        private:

        // Parameters.
        Time dt_; ///< Timestep.
        double initSoc_{0.0};
        double maxSoc_{0.0};
        double maxChargePower_{0.0};
        double maxDischargePower_{0.0};
        double chargeEfficiency_{1.0};
        double dischargeEfficiency_{1.0};

        // Setpoint.
        double requestedPower_{0.0}; ///< Positive = charging.

        // State.
        double soc_{0.0};
        double requestedPDc_{0.0};
    };
}
#endif // BATTERY_DOT_H
