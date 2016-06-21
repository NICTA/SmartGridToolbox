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

#ifndef GEN_DOT_H
#define GEN_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/Component.h>
#include <SgtCore/Event.h>
#include <SgtCore/PowerFlow.h>

namespace Sgt
{
    /// @brief Common abstract base class for a generation at a bus.
    ///
    /// Implement some common functionality for convenience.
    /// @ingroup PowerFlowCore
    class GenAbc : virtual public Component, public HasProperties<GenAbc>
    {
        public:

            SGT_PROPS_INIT(GenAbc);
            SGT_PROPS_INHERIT(GenAbc, Component);

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

            explicit GenAbc(const Phases& phases) :
                phases_(phases)
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
            
            SGT_PROP_GET(phases, GenAbc, const Phases&, phases);

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
            
            SGT_PROP_GET_SET(isInService, GenAbc, bool, isInService, setIsInService);

            /// @}

            /// @name Power injection:
            /// @{

            virtual arma::Col<Complex> S() const final
            {
                return isInService_ ? inServiceS() : arma::Col<Complex>(phases_.size(), arma::fill::zeros);
            }
            
            SGT_PROP_GET(S, GenAbc, arma::Col<Complex>, S);

            virtual arma::Col<Complex> inServiceS() const = 0;

            virtual void setInServiceS(const arma::Col<Complex>& S) = 0;
            
            SGT_PROP_GET_SET(inServiceS, GenAbc, arma::Col<Complex>, inServiceS, setInServiceS);
            
            /// @}

            /// @name Moment of inertia:
            /// @{

            virtual double J() const final
            {
                return isInService_ ? inServiceJ() : 0.0;
            }

            SGT_PROP_GET(J, GenAbc, double, J);

            virtual double inServiceJ() const = 0;
            
            SGT_PROP_GET(inServiceJ, GenAbc, double, inServiceJ);

            /// @}

            /// @name Generation bounds:
            /// @{

            virtual double PMin() const = 0;
            virtual void setPMin(double PMin) = 0;
            SGT_PROP_GET_SET(PMin, GenAbc, double, PMin, setPMin);

            virtual double PMax() const = 0;
            virtual void setPMax(double PMax) = 0;
            SGT_PROP_GET_SET(PMax, GenAbc, double, PMax, setPMax);

            virtual double QMin() const = 0;
            virtual void setQMin(double QMin) = 0;
            SGT_PROP_GET_SET(QMin, GenAbc, double, QMin, setQMin);

            virtual double QMax() const = 0;
            virtual void setQMax(double QMax) = 0;
            SGT_PROP_GET_SET(QMax, GenAbc, double, QMax, setQMax);

            /// @}

            /// @name Generation costs:
            /// @{

            virtual double cStartup() const = 0;
            virtual void setCStartup(double cStartup) = 0;
            SGT_PROP_GET_SET(cStartup, GenAbc, double, cStartup, setCStartup);

            virtual double cShutdown() const = 0;
            virtual void setCShutdown(double cShutdown) = 0;
            SGT_PROP_GET_SET(cShutdown, GenAbc, double, cShutdown, setCShutdown);

            virtual double c0() const = 0;
            virtual void setC0(double c0) = 0;
            SGT_PROP_GET_SET(c0, GenAbc, double, c0, setC0);

            virtual double c1() const = 0;
            virtual void setC1(double c1) = 0;
            SGT_PROP_GET_SET(c1, GenAbc, double, c1, setC1);

            virtual double c2() const = 0;
            virtual void setC2(double c2) = 0;
            SGT_PROP_GET_SET(c2, GenAbc, double, c2, setC2);

            double cost() const;
            SGT_PROP_GET(cost, GenAbc, double, cost);

            /// @}

            /// @name Events.
            /// @{

            /// @brief Event triggered when I go in or out of service.
            virtual Event& isInServiceChanged()
            {
                return isInServiceChanged_;
            }

            /// @brief Event triggered when the amount of generated power changes.
            virtual Event& generationChanged()
            {
                return generationChanged_;
            }
            
            /// @brief Event triggered when the angular momentum changes.
            virtual Event& JChanged()
            {
                return JChanged_;
            }

            /// @brief Event triggered when a setpoint is changed.
            virtual Event& setpointChanged()
            {
                return setpointChanged_;
            }

            /// @}

        private:

            Phases phases_{Phase::BAD};
            bool isInService_{true};

            Event isInServiceChanged_{std::string(sComponentType()) + " : Is in service changed"};
            Event generationChanged_{std::string(sComponentType()) + " : Generation changed"};
            Event JChanged_{std::string(sComponentType()) + " : Angular momentum changed"};
            Event setpointChanged_{std::string(sComponentType()) + " : Setpoint changed"};
    };

    /// @brief A concrete, generic generation at a bus.
    /// @ingroup PowerFlowCore
    class GenericGen : public GenAbc, public HasProperties<GenericGen>
    {
        public:
            
            SGT_PROPS_INIT(GenericGen);
            SGT_PROPS_INHERIT(GenericGen, GenAbc);

            /// @name Static member functions:
            /// @{

            static const std::string& sComponentType()
            {
                static std::string result("generic_gen");
                return result;
            }

            /// @}

            /// @name Lifecycle:
            /// @{

            GenericGen(const std::string& id, const Phases& phases) :
                Component(id),
                GenAbc(phases),
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

            // virtual json toJson() const override; // TODO

            /// @}

            /// @name Power injection:
            /// @{

            virtual arma::Col<Complex> inServiceS() const override
            {
                return S_;
            }

            virtual void setInServiceS(const arma::Col<Complex>& S) override
            {
                S_ = S;
                generationChanged().trigger();
            }

            /// @}

            /// @name Moment of inertia:
            /// @{

            virtual double inServiceJ() const override
            {
                return J_;
            }

            virtual void setInServiceJ(double J)
            {
                J_ = J;
            }
            
            SGT_PROP_GET_SET(inServiceJ, GenericGen, double, inServiceJ, setInServiceJ);

            /// @}

            /// @name Generation bounds:
            /// @{

            virtual double PMin() const override
            {
                return PMin_;
            }

            virtual void setPMin(double PMin) override
            {
                PMin_ = PMin;
                setpointChanged().trigger();
            }

            virtual double PMax() const override
            {
                return PMax_;
            }

            virtual void setPMax(double PMax) override
            {
                PMax_ = PMax;
                setpointChanged().trigger();
            }

            virtual double QMin() const override
            {
                return QMin_;
            }

            virtual void setQMin(double QMin) override
            {
                QMin_ = QMin;
                setpointChanged().trigger();
            }

            virtual double QMax() const override
            {
                return QMax_;
            }

            virtual void setQMax(double QMax) override
            {
                QMax_ = QMax;
                setpointChanged().trigger();
            }

            /// @}

            /// @name Generation costs:
            /// @{

            virtual double cStartup() const override
            {
                return cStartup_;
            }

            virtual void setCStartup(double cStartup) override
            {
                cStartup_ = cStartup;
                setpointChanged().trigger();
            }

            virtual double cShutdown() const override
            {
                return cShutdown_;
            }

            virtual void setCShutdown(double cShutdown) override
            {
                cShutdown_ = cShutdown;
                setpointChanged().trigger();
            }

            virtual double c0() const override
            {
                return c0_;
            }

            virtual void setC0(double c0) override
            {
                c0_ = c0;
                setpointChanged().trigger();
            }

            virtual double c1() const override
            {
                return c1_;
            }

            virtual void setC1(double c1) override
            {
                c1_ = c1;
                setpointChanged().trigger();
            }

            virtual double c2() const override
            {
                return c2_;
            }

            virtual void setC2(double c2) override
            {
                c2_ = c2;
                setpointChanged().trigger();
            }

        private:

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
    };
}

#endif // GEN_DOT_H
