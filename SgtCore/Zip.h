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

#ifndef ZIP_DOT_H
#define ZIP_DOT_H

#include <SgtCore/Component.h>
#include <SgtCore/ComponentCollection.h>
#include <SgtCore/Event.h>
#include <SgtCore/PowerFlow.h>

#include<string>

namespace Sgt
{
    class Bus;

    /// @brief A Zip is a power consumption at a bus with constant impedance / current / complex power components.
    ///
    /// These components are described by an N x N matrix, where N is the number of phases.
    /// Diagonal components represent a power consumption between the phase and ground.
    /// Upper off-diagonal components, (i, j) where j > i, represent a power consumption between two phases. 
    /// Lower off-diagonal components are ignored.
    /// Positive real power = consumption, negative = generation.
    ///
    /// @ingroup PowerFlowCore
    class ZipAbc : virtual public Component
    {
        friend class Network;

        public:
            
            SGT_PROPS_INIT(ZipAbc);
            SGT_PROPS_INHERIT(Component);

            /// @name Static member functions:
            /// @{

            static const std::string& sComponentType()
            {
                static std::string result("zip");
                return result;
            }

            /// @}

            /// @name Lifecycle:
            /// @{

            explicit ZipAbc(const Phases& phases) : Component(""), phases_(phases) {}

            /// @}

            /// @name Component virtual overridden member functions.
            /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            virtual json toJson() const override;

            /// @}

            /// @name Phases
            /// @{

            virtual const Phases& phases() const
            {
                return phases_;
            }
            
            SGT_PROP_GET(phases, phases, const Phases&);

            /// @}

            /// @name In service:
            /// @{

            virtual bool isInService() const
            {
                return isInService_;
            }

            virtual void setIsInService(bool isInService)
            {
                isInService_ = isInService;
            }
            
            SGT_PROP_GET_SET(isInService, isInService, bool, setIsInService, bool);

            /// @}

            /// @name ZIP parameters:
            ///
            /// These are implemented for convenience, so subclasses don't have to reimplement them unless they have
            /// a non-zero value.
            /// @{

            virtual arma::Mat<Complex> YConst() const
            {
                return arma::Mat<Complex>(phases_.size(), phases_.size(), arma::fill::zeros);
            }
            
            SGT_PROP_GET(YConst, YConst, arma::Mat<Complex>);

            /// @brief Constant current component.
            ///
            /// Relative to phase of V. Actual current will be IConst V / |V|, so that S doesn't depend on phase of V.
            virtual arma::Mat<Complex> IConst() const
            {
                return arma::Mat<Complex>(phases_.size(), phases_.size(), arma::fill::zeros);
            }
            
            SGT_PROP_GET(IConst, IConst, arma::Mat<Complex>);

            virtual arma::Mat<Complex> SConst() const
            {
                return arma::Mat<Complex>(phases_.size(), phases_.size(), arma::fill::zeros);
            }
            
            SGT_PROP_GET(SConst, SConst, arma::Mat<Complex>);

            /// @}
            
            /// @name Total Power.
            /// @{
            
            /// @brief Consumed power, by phase components, when in service.
            ///
            /// Does not take into account any unserved power - this is instead taken into account at the bus level.
            arma::Mat<Complex> inServiceS() const;

            /// @brief Consumed power, by phase components, zero if not in service.
            ///
            /// Does not take into account any unserved power - this is instead taken into account at the bus level.
            arma::Mat<Complex> S() const
            {
                return isInService_ 
                    ? inServiceS()
                    : arma::Mat<Complex>(phases_.size(), phases_.size(), arma::fill::zeros);
            }

            /// @brief Total consumed power, when in service.
            ///
            /// Does not take into account any unserved power - this is instead taken into account at the bus level.
            Complex inServiceSTot() const
            {
                return sum(sum(trimatu(inServiceS())));
            }
            
            /// @brief Total consumed power, zero if not in service.
            ///
            /// Does not take into account any unserved power - this is instead taken into account at the bus level.
            Complex STot() const
            {
                return isInService_ ? inServiceSTot() : Complex(0.0, 0.0);
            }
            
            /// @}

            /// @name Events.
            /// @{

            /// @brief Event triggered when I go in or out of service.
            virtual const Event& isInServiceChanged() const
            {
                return isInServiceChanged_;
            }
            
            /// @brief Event triggered when I go in or out of service.
            virtual Event& isInServiceChanged()
            {
                return isInServiceChanged_;
            }

            /// @brief Event triggered when I go in or out of service.
            virtual const Event& injectionChanged() const
            {
                return injectionChanged_;
            }
            
            /// @brief Event triggered when I go in or out of service.
            virtual Event& injectionChanged()
            {
                return injectionChanged_;
            }

            /// @brief Event triggered when I go in or out of service.
            virtual const Event& setpointChanged() const
            {
                return setpointChanged_;
            }
            
            /// @brief Event triggered when I go in or out of service.
            virtual Event& setpointChanged()
            {
                return setpointChanged_;
            }

            /// @}

            /// @name Bus.
            /// @{
            
            ConstComponentPtr<Bus> bus() const
            {
                return bus_;
            }
            
            ComponentPtr<Bus> bus()
            {
                return bus_;
            }

            /// @}

        private:

            Phases phases_;

            bool isInService_{true};

            Event isInServiceChanged_{std::string(sComponentType()) + " : Is in service changed"};
            Event injectionChanged_{std::string(sComponentType()) + " : Injection changed"};
            Event setpointChanged_{std::string(sComponentType()) + " : Setpoint changed"};

            ComponentPtr<Bus> bus_;
    };

    /// @brief A concrete, generic ZIP at a bus.
    /// @ingroup PowerFlowCore
    class GenericZip : public ZipAbc
    {
        public:
            
            SGT_PROPS_INIT(GenericZip);
            SGT_PROPS_INHERIT(ZipAbc);

            /// @name Static member functions:
            /// @{

            static const std::string& sComponentType()
            {
                static std::string result("generic_zip");
                return result;
            }

            /// @}

            /// @name Lifecycle:
            /// @{

            GenericZip(const std::string& id, const Phases& phases);

            /// @}

            /// @name Component virtual overridden member functions.
            /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            // virtual json toJson() const override; TODO

            /// @}

            /// @name ZIP parameters:
            /// @{

            /// @brief Constant admittance component.
            ///
            /// Assumed to be an upper triangular matrix. The diagonal elements are from the bus terminals to ground,
            /// And the off-diagonal elements are between pairs of bus terminals.
            virtual arma::Mat<Complex> YConst() const override
            {
                return YConst_;
            }

            virtual void setYConst(const arma::Mat<Complex>& YConst)
            {
                YConst_ = YConst;
            }
            
            SGT_PROP_GET_SET(YConst, YConst, arma::Mat<Complex>, setYConst, const arma::Mat<Complex>&);

            /// @brief Constant current component.
            ///
            /// Assumed to be an upper triangular matrix. The diagonal elements are from the bus terminals to ground,
            /// And the off-diagonal elements are between pairs of bus terminals. The phase is assumed to be relative
            /// to the phase of V, so that the power is independent of the absolute phase.
            virtual arma::Mat<Complex> IConst() const override
            {
                return IConst_;
            }

            virtual void setIConst(const arma::Mat<Complex>& IConst)
            {
                IConst_ = IConst;
            }
            
            SGT_PROP_GET_SET(IConst, IConst, arma::Mat<Complex>, setIConst, const arma::Mat<Complex>&);

            /// @brief Constant power component.
            ///
            /// Assumed to be an upper triangular matrix. The diagonal elements are from the bus terminals to ground,
            /// And the off-diagonal elements are between pairs of bus terminals.
            virtual arma::Mat<Complex> SConst() const override
            {
                return SConst_;
            }

            virtual void setSConst(const arma::Mat<Complex>& SConst)
            {
                SConst_ = SConst;
            }
            
            SGT_PROP_GET_SET(SConst, SConst, arma::Mat<Complex>, setSConst, const arma::Mat<Complex>&);

            /// @}

        private:

            Phases phases_;

            arma::Mat<Complex> YConst_;
            arma::Mat<Complex> IConst_;
            arma::Mat<Complex> SConst_;
    };
}

#endif // ZIP_DOT_H
