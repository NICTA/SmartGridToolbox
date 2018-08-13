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

#ifndef SIM_COMPONENT_DOT_H
#define SIM_COMPONENT_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/Component.h>
#include <SgtCore/ComponentCollection.h>
#include <SgtCore/Event.h>

namespace Sgt
{
    /// @brief A base class for components of a Simulation.
    /// @ingroup SimCore
    class SimComponent : virtual public Component
    {
        friend class Simulation;

        public:

        /// @name Static member functions:
        /// @{

        static const std::string& sComponentType()
        {
            static std::string result("sim_component");
            return result;
        }

        /// @}

        /// @name Lifecycle:
        /// @{

        SimComponent()
        {
            // Empty.
        }

        virtual ~SimComponent() override = default;

        /// @}

        /// @name Component virtual overridden member functions.
        /// @{

        virtual const std::string& componentType() const override
        {
            return sComponentType();
        }

        // virtual json toJson() const override; // TODO

        /// @}

        /// @name Simulation flow
        /// @{

        /// @brief Initialize state of the object.
        ///
        /// This simply does the initial work needed to set the object up, prior to simulation. The time following
        /// initialization will be set to negative infinity, and the object will be considered to be in an
        /// invalid state. To progress to a valid state, the object will need to undergo an update().
        virtual void initialize() final;

        /// @brief Bring state up to time t.
        virtual void update(const Time& t) final;

        /// @}

        /// @name Virtual methods to be overridden by derived classes.
        /// @{

        /// @brief When is the component scheduled to update?
        ///
        /// An update may occur before this time if the component undergoes a contingent update. If this
        /// occurs, it is possible that validUntil() could change to a later time.
        virtual Time validUntil() const
        {
            return TimeSpecialValues::pos_infin;
        }

        protected:

        /// @brief Reset state of the object, time will be at negative infinity.
        ///
        /// Called by Simulation::initialize(), which should be called before every simulation is started.
        virtual void initializeState()
        {
            // Empty.
        }

        /// @brief Bring state up to time current time.
        ///
        /// Note that, besides normal updates, this function will also be called as part of the initialization
        /// step, to update the time from negative infinity to the simulation start time. This can be checked by
        /// isInitialized().
        virtual void updateState(const Time& t)
        {
            // Empty.
        }
        
        /// @brief Called after all components have finished their final updateState for a timestep.
        virtual void finalizeTimestep()
        {
            // Empty.
        }
        
        /// @brief Called after the last timestep in the simulation. 
        virtual void finalizeSimulation()
        {
            // Empty.
        }

        /// @}

        public:

        /// @name Timestepping
        /// @{

        /// @brief Get the current step for the object.
        Time lastUpdated() const
        {
            return lastUpdated_;
        }

        /// @brief Have we completed initialization?
        ///
        /// true => we've finished both initializeState call and the subsequent initialization call to updateState.
        bool isInitialized() const
        {
            return (lastUpdated_ != TimeSpecialValues::not_a_date_time &&   // InitializeState not yet complete.
                    lastUpdated_ != TimeSpecialValues::neg_infin);          // Initialization update not yet done.
        }

        /// @}

        /// @name Rank
        /// @brief Rank: A < B means B depends on A, not vice-versa, so A should go first.
        /// @{

        /// @brief Get the rank of the object.
        int rank() const
        {
            return rank_;
        }

        /// @brief Set the rank of the object.
        void setRank(int rank)
        {
            rank_ = rank;
        }

        /// @}

        /// @name Dependencies.
        /// @{

        const std::vector<ConstComponentPtr<SimComponent>>& dependencies() const
        {
            return dependencies_;
        }

        /// @brief Add a dependency of one component on another.
        /// @param comp SimComponent on which `dependentComp` depends.
        /// @param forceUpdate Should an update of `comp` force me to do a contingent update? 
        ///
        /// comp is guaranteed to update before me, so long as there aren't any circular dependencies.
        void dependsOn(const ConstComponentPtr<SimComponent>& comp, bool forceUpdate);

        /// @}

        /// @name Events
        /// @{

        /// @brief Triggered just before my update.
        const Event& willUpdate() const {return willUpdate_;}

        /// @brief Triggered just before my update.
        Event& willUpdate() {return willUpdate_;}

        /// @brief Triggered after my update.
        const Event& didUpdate() const {return didUpdate_;}

        /// @brief Triggered after my update.
        Event& didUpdate() {return didUpdate_;}

        /// @brief Triggered when I am flagged for future update.
        const Event& needsUpdate() const {return needsUpdate_;}

        /// @brief Triggered when I am flagged for future update.
        Event& needsUpdate() {return needsUpdate_;}

        /// @brief Triggered when I am about to update to a new timestep.
        const Event& willStartNewTimestep() const {return willStartNewTimestep_;}

        /// @brief Triggered when I am about to update to a new timestep.
        Event& willStartNewTimestep() {return willStartNewTimestep_;}

        /// @brief Triggered when I just updated, completing the current timestep.
        const Event& didCompleteTimestep() const {return didCompleteTimestep_;}

        /// @brief Triggered when I just updated, completing the current timestep.
        Event& didCompleteTimestep() {return didCompleteTimestep_;}

        /// @}

        private:

        Time lastUpdated_{TimeSpecialValues::not_a_date_time};
        ///< The time to which I am up to date
        std::vector<ConstComponentPtr<SimComponent>> dependencies_;
        ///< I depend on these.
        int rank_{-1};
        ///< Evaluation rank, based on weak ordering.

        Event willUpdate_{std::string(componentType()) + ": Will update"};
        ///< Triggered immediately prior to upddate.
        Event didUpdate_{std::string(componentType()) + ": Did update"};
        ///< Triggered immediately post update.
        Event needsUpdate_{std::string(componentType()) + ": Needs update"};
        ///< Triggered when I need to be updated.
        Event willStartNewTimestep_{std::string(componentType()) + ": Will start new timestep"};
        ///< Triggered immediately prior to time advancing.
        Event didCompleteTimestep_{std::string(componentType()) + ": Did complete timestep"};
        ///< Triggered just after fully completing a timestep.

        Action insertContingentUpdateAction_;
    };

    template<typename T = SimComponent> using SimComponentPtr = ComponentPtr<SimComponent, T>;
    template<typename T = SimComponent> using ConstSimComponentPtr = ConstComponentPtr<SimComponent, T>;
}

#endif // SIM_COMPONENT_DOT_H
