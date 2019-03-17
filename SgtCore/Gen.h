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

#ifndef GEN_DOT_H
#define GEN_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/Component.h>
#include <SgtCore/ComponentCollection.h>
#include <SgtCore/Event.h>
#include <SgtCore/PowerFlow.h>

namespace Sgt
{
    class Bus;

    /// @brief Generation at a bus.
    /// @ingroup PowerFlowCore
    class Gen : virtual public Component
    {
        friend class Network;

        public:

        SGT_PROPS_INIT(Gen);
        SGT_PROPS_INHERIT(Component);

        /// @name Static member functions:
        /// @{

        static const std::string& sComponentType()
        {
            static std::string result("gen");
            return result;
        }

        /// @}

        /// @name Lifecycle:
        /// @{

        Gen(const std::string& id, const Phases& phases) :
            Component(id),
            phases_(phases),
            S_(phases.size(), arma::fill::zeros)
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

        virtual json toJson() const override;

        /// @}

        /// @name Phase accessors:
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

        virtual void setIsInService(bool isInService);

        SGT_PROP_GET_SET(isInService, isInService, bool, setIsInService, bool);

        /// @}

        /// @name Power injection:
        /// @{

        virtual arma::Col<Complex> S() const final
        {
            return isInService_ ? inServiceS() : arma::Col<Complex>(phases_.size(), arma::fill::zeros);
        }

        SGT_PROP_GET(S, S, arma::Col<Complex>);

        virtual arma::Col<Complex> inServiceS() const
        {
            return S_;
        }

        virtual void setInServiceS(const arma::Col<Complex>& S)
        {
            S_ = S;
            generationChanged().trigger();
        }

        SGT_PROP_GET_SET(inServiceS, inServiceS, arma::Col<Complex>, setInServiceS, const arma::Col<Complex>&);

        /// @}

        /// @name Moment of inertia:
        /// @{

        virtual double J() const final
        {
            return isInService_ ? inServiceJ() : 0.0;
        }

        SGT_PROP_GET(J, J, double);

        virtual double inServiceJ() const
        {
            return J_;
        }

        virtual void setInServiceJ(double J)
        {
            J_ = J;
        }

        SGT_PROP_GET_SET(inServiceJ, inServiceJ, double, setInServiceJ, double);

        /// @}

        /// @name Generation bounds:
        /// @{

        virtual double PMin() const
        {
            return PMin_;
        }

        virtual void setPMin(double PMin)
        {
            PMin_ = PMin;
            setpointChanged().trigger();
        }

        SGT_PROP_GET_SET(PMin, PMin, double, setPMin, double);

        virtual double PMax() const
        {
            return PMax_;
        }

        virtual void setPMax(double PMax)
        {
            PMax_ = PMax;
            setpointChanged().trigger();
        }

        SGT_PROP_GET_SET(PMax, PMax, double, setPMax, double);

        virtual double QMin() const
        {
            return QMin_;
        }

        virtual void setQMin(double QMin)
        {
            QMin_ = QMin;
            setpointChanged().trigger();
        }

        SGT_PROP_GET_SET(QMin, QMin, double, setQMin, double);

        virtual double QMax() const
        {
            return QMax_;
        }

        virtual void setQMax(double QMax)
        {
            QMax_ = QMax;
            setpointChanged().trigger();
        }

        SGT_PROP_GET_SET(QMax, QMax, double, setQMax, double);

        /// @}

        /// @name Generation costs:
        /// @{

        virtual double cStartup() const
        {
            return cStartup_;
        }

        virtual void setCStartup(double cStartup)
        {
            cStartup_ = cStartup;
            setpointChanged().trigger();
        }

        SGT_PROP_GET_SET(cStartup, cStartup, double, setCStartup, double);

        virtual double cShutdown() const
        {
            return cShutdown_;
        }

        virtual void setCShutdown(double cShutdown)
        {
            cShutdown_ = cShutdown;
            setpointChanged().trigger();
        }

        SGT_PROP_GET_SET(cShutdown, cShutdown, double, setCShutdown, double);

        virtual double c0() const
        {
            return c0_;
        }

        virtual void setC0(double c0)
        {
            c0_ = c0;
            setpointChanged().trigger();
        }

        SGT_PROP_GET_SET(c0, c0, double, setC0, double);

        virtual double c1() const
        {
            return c1_;
        }

        virtual void setC1(double c1)
        {
            c1_ = c1;
            setpointChanged().trigger();
        }

        SGT_PROP_GET_SET(c1, c1, double, setC1, double);

        virtual double c2() const
        {
            return c2_;
        }

        virtual void setC2(double c2)
        {
            c2_ = c2;
            setpointChanged().trigger();
        }

        SGT_PROP_GET_SET(c2, c2, double, setC2, double);

        double cost() const;
        SGT_PROP_GET(cost, cost, double);

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

        /// @brief Event triggered when the amount of generated power changes.
        virtual const Event& generationChanged() const
        {
            return generationChanged_;
        }

        /// @brief Event triggered when the amount of generated power changes.
        virtual Event& generationChanged()
        {
            return generationChanged_;
        }

        /// @brief Event triggered when the angular momentum changes.
        virtual const Event& JChanged() const
        {
            return JChanged_;
        }

        /// @brief Event triggered when the angular momentum changes.
        virtual Event& JChanged()
        {
            return JChanged_;
        }

        /// @brief Event triggered when a setpoint is changed.
        virtual const Event& setpointChanged() const
        {
            return setpointChanged_;
        }

        /// @brief Event triggered when a setpoint is changed.
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

        Phases phases_{Phase::BAD};
        bool isInService_{true};

        arma::Col<Complex> S_;

        double J_{0.0};

        double PMin_{-infinity};
        double PMax_{infinity};
        double QMin_{-infinity};
        double QMax_{infinity};

        double cStartup_{0.0};
        double cShutdown_{0.0};
        double c0_{0.0};
        double c1_{0.0};
        double c2_{0.0};

        Event isInServiceChanged_{std::string(componentType()) + ": Is in service changed"};
        Event generationChanged_{std::string(componentType()) + ": Generation changed"};
        Event JChanged_{std::string(componentType()) + ": Angular momentum changed"};
        Event setpointChanged_{std::string(componentType()) + ": Setpoint changed"};

        ComponentPtr<Bus> bus_;
    };
}

#endif // GEN_DOT_H
