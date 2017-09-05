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

#ifndef DC_POWER_SOURCE_DOT_H
#define DC_POWER_SOURCE_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Component.h>

namespace Sgt
{
    /// @brief DC power source.
    ///
    /// Abstract base class for any object that can provide a source of DC power, i.e. a single real power.
    class DcPowerSourceAbc : virtual public SimComponent
    {
        public:

            /// @name Static member functions:
            /// @{

            static const std::string& sComponentType()
            {
                static std::string result("dc_power_source");
                return result;
            }

            /// @}

            /// @name Lifecycle.
            /// @{

            virtual ~DcPowerSourceAbc() = default;

            /// @}

            /// @name DC Power.
            /// @{

            /// @brief DC power. +ve = generation.
            virtual double PDc() const = 0;

            /// @}

            /// @name Events.
            /// @{

            /// @brief Event triggered when the DC power changes.
            virtual const Event& dcPowerChanged() const
            {
                return dcPowerChanged_;
            }

            /// @}

        private:

            Event dcPowerChanged_{std::string(componentType()) + ": DC power changed"};
    };

    class GenericDcPowerSource : public DcPowerSourceAbc
    {
        public:

            /// @name Static member functions:
            /// @{

            static const std::string& sComponentType()
            {
                static std::string result("generic_dc_power_source");
                return result;
            }

            /// @}

            /// @name Lifecycle
            /// @{

            GenericDcPowerSource(const std::string& id) : Component(id), PDc_(0.0) {}

            /// @}

            /// @name Component virtual overridden member functions.
            /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            virtual json toJson() const override
            {
                json j = this->SimComponent::toJson();
                j[sComponentType()] = {{"DC_power", PDc()}};
                return j;
            }

            /// @}

            /// @name SimComponent virtual overridden member functions.
            /// @{

        protected:

            /// @brief Reset state of the object, time will be at negative infinity.
            
            virtual void initializeState() override
            {
                PDc_ = 0.0;
            }

            /// @}

        public:

            /// @name DC Power.
            /// @{

            virtual double PDc() const override {return PDc_;}
            void setPDc(double PDc) {PDc_ = PDc; dcPowerChanged().trigger();}

            /// @}

        private:
            double PDc_;
    };
}

#endif // DC_POWER_SOURCE_DOT_H
