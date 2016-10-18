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

            static std::function<double ()> get(const ConstComponentPtr<Bus>& targ, const std::string& key)
            {
                if      (key == "V0")
                {
                    return [targ](){return std::abs(targ->V()(0));};
                }
                else if (key == "V1")
                {
                    return [targ](){return std::abs(targ->V()(1));};
                }
                else if (key == "V2")
                {
                    return [targ](){return std::abs(targ->V()(1));};
                }
                else if (key == "V01")
                {
                    return [targ](){auto V = targ->V(); return std::abs(V(1) - V(0));};
                }
                else if (key == "V12")
                {
                    return [targ](){auto V = targ->V(); return std::abs(V(2) - V(1));};
                }
                else if (key == "V20")
                {
                    return [targ](){auto V = targ->V(); return std::abs(V(0) - V(2));};
                }
                else if (key == "VRms")
                {
                    return [targ](){return rms(targ->V());};
                }
                else
                {
                    sgtError(sComponentType() + " : no such control bus function.");
                }
            }

            static std::function<void (double)> setMag(const ComponentPtr<BranchAbc>& targ,
                    const Property<Complex, Complex>& prop)
            {
                return [targ, &prop](double val)
                    {
                        Complex x = prop.get(*targ);
                        x = val * x / std::abs(x);
                        prop.set(*targ, x);
                    };
            }

            static std::function<void (double)> setVal(const ComponentPtr<BranchAbc>& targ,
                    const Setter<double>& setter)
            {
                return [targ, &setter](double val) {setter.set(*targ, val);};
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
            
            /// @name My functions.
            /// @{
            
            const std::vector<double>& taps() const
            {
                return taps_;
            }

            std::size_t tapSetting() const
            {
                return setting_;
            }

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
