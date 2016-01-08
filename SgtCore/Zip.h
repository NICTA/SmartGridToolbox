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
#include <SgtCore/Event.h>
#include <SgtCore/PowerFlow.h>

#include<string>

namespace Sgt
{
    /// @brief A Zip is an injection into a bus with constant impedance / current / complex power components.
    ///
    /// Implement some common functionality for convenience.
    /// @ingroup PowerFlowCore
    class ZipAbc : virtual public Component
    {
        public:

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

            ZipAbc(const Phases& phases) : Component(""), phases_(phases) {}

            /// @}

            /// @name Component virtual overridden member functions.
            /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            virtual json asJson() const override;

            /// @}

            /// @name Phases
            /// @{

            virtual const Phases& phases() const
            {
                return phases_;
            }

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

            /// @}

            /// @name ZIP parameters:
            ///
            /// These are implemented for convenience, so subclasses don't have to reimplement them unless they have
            /// a non-zero value.
            /// @{

            virtual arma::Col<Complex> YConst() const
            {
                return arma::Col<Complex>(phases_.size(), arma::fill::zeros);
            }

            /// @brief Constant current component.
            ///
            /// Relative to phase of V. Actual current will be IConst V / |V|, so that S doesn't depend on phase of V.
            virtual arma::Col<Complex> IConst() const
            {
                return arma::Col<Complex>(phases_.size(), arma::fill::zeros);
            }

            virtual arma::Col<Complex> SConst() const
            {
                return arma::Col<Complex>(phases_.size(), arma::fill::zeros);
            }

            /// @}

            /// @name Events.
            /// @{

            /// @brief Event triggered when I go in or out of service.
            virtual Event& isInServiceChanged()
            {
                return isInServiceChanged_;
            }

            /// @brief Event triggered when I go in or out of service.
            virtual Event& injectionChanged()
            {
                return injectionChanged_;
            }

            /// @brief Event triggered when I go in or out of service.
            virtual Event& setpointChanged()
            {
                return setpointChanged_;
            }

            /// @}

        private:

            Phases phases_;

            bool isInService_;

            Event isInServiceChanged_{std::string(sComponentType()) + " : Is in service changed"};
            Event injectionChanged_{std::string(sComponentType()) + " : Injection changed"};
            Event setpointChanged_{std::string(sComponentType()) + " : Setpoint changed"};
    };

    /// @brief A concrete, generic ZIP at a bus.
    /// @ingroup PowerFlowCore
    class GenericZip : public ZipAbc
    {
        public:

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

            // virtual json asJson() const override; TODO

            /// @}

            /// @name ZIP parameters:
            /// @{

            virtual arma::Col<Complex> YConst() const override
            {
                return YConst_;
            }

            virtual void setYConst(const arma::Col<Complex>& YConst)
            {
                YConst_ = YConst;
            }

            virtual arma::Col<Complex> IConst() const override
            {
                return IConst_;
            }

            virtual void setIConst(const arma::Col<Complex>& IConst)
            {
                IConst_ = IConst;
            }

            virtual arma::Col<Complex> SConst() const override
            {
                return SConst_;
            }

            virtual void setSConst(const arma::Col<Complex>& SConst)
            {
                SConst_ = SConst;
            }

            /// @}

        private:

            Phases phases_;

            arma::Col<Complex> YConst_;
            arma::Col<Complex> IConst_;
            arma::Col<Complex> SConst_;
    };
}

#endif // ZIP_DOT_H
