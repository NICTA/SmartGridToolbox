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

#include <SgtSim/SimComponent.h>

#include <SgtCore/Bus.h>
#include <SgtCore/Transformer.h>

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

            TapChanger(
                    const std::string& id,
                    const ConstComponentPtr<BranchAbc, TransformerAbc>& trans,
                    const std::vector<double>& taps,
                    double setpoint,
                    double tolerance,
                    arma::uword ctrlSideIdx,
                    arma::uword windingIdx,
                    arma::uword ratioIdx,
                    bool hasLdc = false,
                    Complex ZLdc = {0.0, 0.0},
                    Complex topFactorI = 1.0);

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

            double setpoint() const
            {
                return setpoint_;
            }
            
            double tolerance() const
            {
                return tolerance_;
            }
            
            double get() const;

            void set(double val);

            /// @}

        private:
            const ConstComponentPtr<BranchAbc, TransformerAbc> trans_; // Target transformer.

            std::vector<double> taps_;
            double setpoint_;
            double tolerance_{0.02};
          
            Time prevTimestep_{posix_time::neg_infin};
            size_t iter_{0};
            std::size_t setting_{0};
            double val_{0};

            arma::uword ctrlSideIdx_; // Secondary (0) or primary (1)
            arma::uword windingIdx_; // Which winding does tap changer act on, e.g. 1, 2, or 3?
            arma::uword ratioIdx_; // Idx into offNomRatio vector to adjust.
            bool hasLdc_; // Is there line drop compensation?
            Complex ZLdc_; // Impedance for LDC.
            Complex topFactorI_; // LDC topological factor that projects I in winding to a line current.
    };
}

#endif // TAP_CHANGER_DOT_H
