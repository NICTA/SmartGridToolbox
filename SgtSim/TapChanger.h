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

#ifndef TAP_CHANGER_DOT_H
#define TAP_CHANGER_DOT_H

#include <SgtSim/SimBus.h>

#include <SgtCore/Bus.h>

#include <functional>

namespace Sgt
{
    //  Init :   
    //      set().
    //      Device fires event
    //      network updates etc.
    //  Bus updates :
    //      contingent update of this.
    //  Update:
    //      get() to assess setpoint; bus is already updated. 
    //      set() if necessary :
    //          bus setpoint changed which triggers another cycle.
    class TapChanger : public SimComponent
    {
        public:
            /// @name Static member functions:
            /// @{

            static const std::string& sComponentType()
            {
                static std::string result("tap_changer");
                return result;
            }

            /// @}

            /// @name Lifecycle:
            /// @{

            TapChanger(const std::string& id, const ConstComponentPtr<Bus>& bus,
                    const std::vector<double>& taps, double setpoint, double tolerance,
                    std::function<double ()> get, std::function<void (double)> set) :
                Component(id),
                taps_(taps),
                setpoint_(setpoint),
                tolerance_(tolerance),
                get_(get),
                set_(set)
            {
                needsUpdate().addTrigger(bus->voltageUpdated());
            }

            TapChanger(const std::string& id, const ConstComponentPtr<Bus>& bus,
                    const std::vector<double>& taps, double setpoint, double tolerance,
                    const Property<double, double>& prop, const ComponentPtr<BranchAbc>& targ) :
                TapChanger(
                        id, bus, taps, setpoint, tolerance,
                        [&prop, targ]()->double{return prop.get(*targ);},
                        [&prop, targ](double val){prop.set(*targ, val);})
            {
                // Empty.
            }

            virtual ~TapChanger() = default;

            /// @}

            /// @name Component virtual overridden member functions.
            /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            // virtual json toJson() const override; TODO

            /// @}

            /// @name SimComponent virtual overridden member functions.
            /// @{

            virtual Time validUntil() const override
            {
                return posix_time::pos_infin; // Never undergo a scheduled update.
            }

            virtual void initializeState() override;

            virtual void updateState(Time t) override;

            /// @}

        private:

            bool isFirstStep_{false};
            bool isDone_{false};

            std::vector<double> taps_;
            std::size_t idx_;
            double setpoint_;
            double tolerance_{0.02};
            double val_{1e6}; // Always force at least one round.
            std::function<double ()> get_;
            std::function<void (double)> set_;
    };
}

#endif // TAP_CHANGER_DOT_H
