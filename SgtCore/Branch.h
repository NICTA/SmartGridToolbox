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

#ifndef BRANCH_DOT_H
#define BRANCH_DOT_H

#include <SgtCore/Component.h>
#include <SgtCore/Components.h>
#include <SgtCore/Event.h>
#include <SgtCore/PowerFlow.h>

#include <iostream>

namespace Sgt
{
    class Bus;

    /// @brief Common abstract base class for a branch.
    ///
    /// Implement some common functionality for convenience.
    /// @ingroup PowerFlowCore
    class BranchAbc : virtual public Component
    {
        friend class Network;

        public:

            SGT_PROPS_INIT(BranchAbc);
            SGT_PROPS_INHERIT(Component);

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("branch");
                return result;
            }

        /// @}

        /// @name Lifecycle:
        /// @{

            BranchAbc(const Phases& phases0, const Phases& phases1);

        /// @}

        /// @name Component virtual overridden member functions.
        /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            virtual json toJson() const override;

        /// @}

        /// @name Phase accessors:
        /// @{

            virtual const Phases& phases0() const
            {
                return phases0_;
            }

            SGT_PROP_GET(phases0, phases0, const Phases&);

            virtual const Phases& phases1() const
            {
                return phases1_;
            }

            SGT_PROP_GET(phases1, phases1, const Phases&);

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
                isInServiceChanged_.trigger();
            }

            SGT_PROP_GET_SET(isInService, isInService, bool, setIsInService, bool);

        /// @}

        /// @name Nodal admittance matrix (Y):
        /// @{

            virtual arma::Mat<Complex> Y() const final
            {
                size_t n = phases0_.size() + phases1_.size();
                return isInService_ ? inServiceY() : arma::Mat<Complex>(n, n, arma::fill::zeros);
            }

            SGT_PROP_GET(Y, Y, arma::Mat<Complex>);

            /// @brief The admittance whenever isInService.
            virtual arma::Mat<Complex> inServiceY() const = 0;

            SGT_PROP_GET(inServiceY, inServiceY, arma::Mat<Complex>);

        /// @}

        /// @name Events.
        /// @{

            /// @brief Event triggered when I go in or out of service.
            virtual Event& isInServiceChanged()
            {
                return isInServiceChanged_;
            }

            /// @brief Event triggered when my admittance changes.
            virtual Event& admittanceChanged()
            {
                return admittanceChanged_;
            }

        /// @}

        /// @name attached buses:
        /// @{

            ConstComponentPtr<Bus> bus0() const
            {
                return bus0_;
            }
            
            ComponentPtr<Bus> bus0()
            {
                return bus0_;
            }

            ConstComponentPtr<Bus> bus1() const
            {
                return bus1_;
            }
            
            ComponentPtr<Bus> bus1()
            {
                return bus1_;
            }

        /// @}

        private:

            Phases phases0_; ///< Phases on bus 0.
            Phases phases1_; ///< Phases on bus 1.

            bool isInService_; ///< Am I in service?

            Event isInServiceChanged_{sComponentType() + " : Is in service changed"};
            Event admittanceChanged_{sComponentType() + " : Admittance changed"};
            
            ComponentPtr<Bus> bus0_;
            ComponentPtr<Bus> bus1_;
    };

    /// @brief A concrete, generic branch.
    /// @ingroup PowerFlowCore
    class GenericBranch : public BranchAbc
    {
        public:

            SGT_PROPS_INIT(GenericBranch);
            SGT_PROPS_INHERIT(BranchAbc);

        /// @name Static member functions:
        /// @{

            static const std::string& sComponentType()
            {
                static std::string result("generic_branch");
                return result;
            }

        /// @}

        /// @name Lifecycle:
        /// @{

            GenericBranch(const std::string& id, const Phases& phases0, const Phases& phases1) :
                Component(id),
                BranchAbc(phases0, phases1),
                Y_(phases0.size() + phases1.size(),
                phases0.size() + phases1.size(),
                arma::fill::zeros)
            {
                // Empty.
            }

        /// @}

        /// @name Component virtual overridden member functions.
        /// @{

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            // virtual json toJson() const override; // TODO
        /// @}

        /// @name BranchAbc virtual overridden member functions.
        /// @{

            virtual arma::Mat<Complex> inServiceY() const override
            {
                return Y_;
            }

        /// @}

        /// @name Setter for Y.
        /// @{

            void setInServiceY(const arma::Mat<Complex>& Y)
            {
                Y_ = Y;
            }

        /// @}

        protected:

        private:

            arma::Mat<Complex> Y_;
    };
}

#endif // BRANCH_DOT_H
