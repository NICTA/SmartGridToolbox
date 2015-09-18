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
    /// charge in units of MWh.
    class Battery : public DcPowerSourceAbc
    {

        /// @name Lifecycle
        /// @{

        public:
            
            Battery(const std::string& id) :
                Component(id),
                dt_(posix_time::minutes(5)),
                initCharge_(0.0),
                maxChargePower_(0.0),
                maxDischargePower_(0.0),
                chargeEfficiency_(0.0),
                dischargeEfficiency_(0.0),
                requestedPower_(0.0),
                charge_(0.0)
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

            // virtual void print(std::ostream& os) const override; // TODO

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
                charge_ = initCharge_;
            }

            virtual void updateState(Time t) override;

        /// @}

        /// @name Overridden member functions from DcPowerSourceAbc.
        /// @{

        public:
            
            virtual double PDc() const override; ///< Positive = charging.

        /// @}

        /// @name Battery specific member functions.
        /// @{

            Time dt() {return dt_;}
            void set_dt(Time val) {dt_ = val; needsUpdate().trigger();}

            double initCharge() {return initCharge_;}
            void setInitCharge(double val) {initCharge_ = val; needsUpdate().trigger();}

            double maxCharge() {return maxCharge_;}
            void setMaxCharge(double val) {maxCharge_ = val; needsUpdate().trigger();}

            double maxChargePower() {return maxChargePower_;}
            void setMaxChargePower(double val) {maxChargePower_ = val; needsUpdate().trigger();}

            double maxDischargePower() {return maxDischargePower_;}
            void setMaxDischargePower(double val) {maxDischargePower_ = val; needsUpdate().trigger();}

            double chargeEfficiency() {return chargeEfficiency_;}
            void setChargeEfficiency(double val) {chargeEfficiency_ = val; needsUpdate().trigger();}

            double dischargeEfficiency() {return dischargeEfficiency_;}
            void setDischargeEfficiency(double val) {dischargeEfficiency_ = val; needsUpdate().trigger();}

            double charge() {return charge_;}

            double requestedPower() {return requestedPower_;}
            void setRequestedPower(double val) {requestedPower_ = val; needsUpdate().trigger();}

            double internalPower();

        /// @}

        private:
            // Parameters.
            Time dt_; ///< Timestep.
            double initCharge_;
            double maxCharge_;
            double maxChargePower_;
            double maxDischargePower_;
            double chargeEfficiency_;
            double dischargeEfficiency_;

            // Setpoint.
            double requestedPower_; ///< Positive = charging.

            // State.
            double charge_;
    };
}
#endif // BATTERY_DOT_H
