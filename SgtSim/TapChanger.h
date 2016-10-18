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

            static const std::map<std::string, ConcreteGetter<Bus, double>>& getters()
            {
                static const std::map<std::string, ConcreteGetter<Bus, double>>& getters =
                    {
                        {"V0", [](const Bus& bus){return std::abs(bus.V()(0));}},
                        {"V1", [](const Bus& bus){return std::abs(bus.V()(1));}},
                        {"V2", [](const Bus& bus){return std::abs(bus.V()(2));}},
                        {"V01", [](const Bus& bus){auto V = bus.V(); return std::abs(V(1) - V(0));}},
                        {"V12", [](const Bus& bus){auto V = bus.V(); return std::abs(V(2) - V(1));}},
                        {"V21", [](const Bus& bus){auto V = bus.V(); return std::abs(V(1) - V(2));}},
                        {"VRms", [](const Bus& bus){return rms(bus.V());}}
                    };
                return getters;
            }

            /// @}

            /// @name Lifecycle:
            /// @{

            TapChanger(const std::string& id, const std::vector<double>& taps, double setpoint, double tolerance,
                    const ConstComponentPtr<Bus>& controlBus,
                    std::function<double ()> get, std::function<void (double)> set) :
                Component(id),
                taps_(taps),
                setpoint_(setpoint),
                tolerance_(tolerance),
                get_(get),
                set_(set)
            {
                needsUpdate().addTrigger(controlBus->voltageUpdated());
            }

            TapChanger(const std::string& id, const std::vector<double>& taps, double setpoint, double tolerance,
                    const ConstComponentPtr<Bus>& controlBus, const ComponentPtr<BranchAbc>& targ,
                    const Getter<double>& getter, const Setter<double>& setter) :
                TapChanger(
                        id, taps, setpoint, tolerance, controlBus,
                        [&getter, targ]()->double{return getter.get(*targ);},
                        [&setter, targ](double d){setter.set(*targ, d);})
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
            std::vector<double> taps_;
            double setpoint_;
            double tolerance_{0.02};
            std::function<double ()> get_;
            std::function<void (double)> set_;
          
            Time prevTimestep_{posix_time::neg_infin};
            size_t iter_{0};
            std::size_t setting_{0};
            double val_{0};
    };
}

#endif // TAP_CHANGER_DOT_H
