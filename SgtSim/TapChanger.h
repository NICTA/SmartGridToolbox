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

#include <SgtSim/Heartbeat.h>
#include <SgtSim/SimComponent.h>
#include <SgtSim/TimeSeries.h>

#include <SgtCore/Bus.h>
#include <SgtCore/Transformer.h>

#include <functional>

namespace Sgt
{
    class TapChangerAbc : virtual public SimComponent
    {
        public:

        /// @name Lifecycle:
        /// @{

        TapChangerAbc(
                const ComponentPtr<BranchAbc, TransformerAbc>& trans,
                arma::uword ratioIdx,
                double minTapRatio,
                double maxTapRatio,
                int minTap,
                std::size_t nTaps);

        /// @}

        /// @name Component virtual overridden member functions.
        /// @{

        // virtual json toJson() const override; TODO

        /// @}

        /// @name My functions.
        /// @{

        int tap() const
        {
            return tap_;
        }
        
        double tapRatio() const
        {
            return tapRatios_[tapIdx()];
        }
        
        const std::vector<double>& tapRatios() const
        {
            return tapRatios_;
        }

        /// @}

        protected:

        std::size_t tapIdx() const
        {
            return static_cast<std::size_t>(tap_ - minTap_);
        }

        void setTap(int tap)
        {
            tap_ = tap;
            trans_->setOffNomRatio(tapRatio(), ratioIdx_);
        }

        protected:

        const ComponentPtr<BranchAbc, TransformerAbc> trans_; // Target transformer.
        arma::uword ratioIdx_; // Idx into offNomRatio vector to adjust.

        std::vector<double> tapRatios_;

        int tap_{0};
        int minTap_{0};
    };

    //  Init :   
    //      setTap(tap).
    //      Device fires event
    //      network updates etc.
    //  Bus updates :
    //      contingent update of this.
    //  Update:
    //      getCtrlV() to assess setpoint; bus is already updated. 
    //      setTap(tap) if necessary :
    //          bus setpoint changed which triggers another cycle.

    class AutoTapChanger : public TapChangerAbc
    {
        public:

        /// @name Static member functions:
        /// @{

        static const std::string& sComponentType()
        {
            static std::string result("auto_tap_changer");
            return result;
        }

        /// @}

        /// @name Lifecycle:
        /// @{

        AutoTapChanger(
                const std::string& id,
                const ComponentPtr<BranchAbc, TransformerAbc>& trans,
                arma::uword ratioIdx,
                double minTapRatio,
                double maxTapRatio,
                int minTap,
                std::size_t nTaps,
                double setpoint,
                double tolerance,
                arma::uword ctrlSideIdx,
                arma::uword ctrlWindingIdx,
                bool hasLdc = false,
                Complex ZLdc = {0.0, 0.0},
                Complex ldcTopFactorI = 1.0);

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
            return TimeSpecialValues::pos_infin; // Never undergo a scheduled update.
        }

        virtual void initializeState() override;

        virtual void updateState(const Time& t) override;

        /// @}

        /// @name My functions.
        /// @{

        double setpoint() const
        {
            return setpoint_;
        }

        double tolerance() const
        {
            return tolerance_;
        }

        protected:

        double getCtrlV() const;

        /// @}

        private:

        double setpoint_;
        double tolerance_{0.02};
        arma::uword ctrlSideIdx_; // Secondary (0) or primary (1)
        arma::uword ctrlWindingIdx_; // Which winding does tap changer act on, e.g. 1, 2, or 3?
        bool hasLdc_; // Is there line drop compensation?
        Complex ZLdc_; // Impedance for LDC.
        Complex ldcTopFactorI_; // LDC topological factor that projects I in winding to a line current.
        
        Time prevTimestep_{TimeSpecialValues::neg_infin};
        size_t iter_{0};
        double ctrlV_{0};
    };
    
    class TimeSeriesTapChanger : public Heartbeat, public TapChangerAbc
    {
        public:

        /// @name Static member functions:
        /// @{

        static const std::string& sComponentType()
        {
            static std::string result("time_series_tap_changer");
            return result;
        }

        /// @}

        /// @name Lifecycle:
        /// @{

        TimeSeriesTapChanger(
                const std::string& id,
                const ComponentPtr<BranchAbc, TransformerAbc>& trans,
                arma::uword ratioIdx,
                double minTapRatio,
                double maxTapRatio,
                int minTap,
                std::size_t nTaps,
                ConstTimeSeriesPtr<StepwiseTimeSeries<Time, double>> series,
                const Time& dt
                ) :
            Component(id),
            Heartbeat(dt),
            TapChangerAbc(trans, ratioIdx, minTapRatio, maxTapRatio, minTap, nTaps),
            series_(series)
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

        // virtual json toJson() const override; TODO

        /// @}

        /// @name SimComponent virtual overridden member functions.
        /// @{

        virtual void updateState(const Time& t) override;

        /// @}

        private:

        ConstTimeSeriesPtr<StepwiseTimeSeries<Time, double>> series_;
    };
}

#endif // TAP_CHANGER_DOT_H
