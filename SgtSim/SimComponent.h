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
#include <SgtCore/Event.h>

namespace Sgt
{
    /// @brief A base class for components of a Simulation.
    /// @ingroup SimCore
    class SimComponent : virtual public Component
    {
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

            SimComponent() : Component("")
            {
                // Empty.
            }

            virtual ~SimComponent() = default;

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
            virtual void update(Time t) final;

            /// @}

            /// @name Virtual methods to be overridden by derived classes.
            /// @{

        public:

            /// @brief When is the component scheduled to update?
            ///
            /// An update may occur before this time if the component undergoes a contingent update. If this
            /// occurs, it is possible that validUntil() could change to a later time.
            virtual Time validUntil() const
            {
                return posix_time::pos_infin;
            }

        protected:

            /// @brief Reset state of the object, time will be at negative infinity.
            virtual void initializeState()
            {
                // Empty.
            }

            /// @brief Bring state up to time current time.
            virtual void updateState(Time t)
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
            /// @param dependentComp SimComponent that depends on `comp`.
            /// @param forceUpdate Should an update of `comp` force a contingent update of `dependentComp`?
            /// If there is an update of `dependentComp`, then it is guaranteed to occur after `comp`, so long as
            /// there aren't any circular dependencies. If `forceUpdate` is true, then an update of `comp`
            /// will guarantee (using a contingent update) that `dependentComp` also updates.
            static void addDependency(const ComponentPtr<SimComponent> comp, 
                    SimComponent& dependentComp, bool forceUpdate);

            /// @}

            /// @name Events
            /// @{

            /// @brief Triggered just before my update.
            Event& willUpdate() {return willUpdate_;}

            /// @brief Triggered after my update.
            Event& didUpdate() {return didUpdate_;}

            /// @brief Triggered when I am flagged for future update.
            Event& needsUpdate() {return needsUpdate_;}

            /// @brief Triggered when I am about to update to a new timestep.
            Event& willStartNewTimestep() {return willStartNewTimestep_;}

            /// @brief Triggered when I just updated, completing the current timestep.
            Event& didCompleteTimestep() {return didCompleteTimestep_;}

            /// @}

        private:

            Time lastUpdated_{posix_time::not_a_date_time};
            ///< The time to which I am up to date
            std::vector<ConstComponentPtr<SimComponent>> dependencies_;
            ///< I depend on these.
            int rank_{-1};
            ///< Evaluation rank, based on weak ordering.
            Event willUpdate_{std::string(sComponentType()) + "Will update"};
            ///< Triggered immediately prior to upddate.
            Event didUpdate_{std::string(sComponentType()) + "Did update"};
            ///< Triggered immediately post update.
            Event needsUpdate_{std::string(sComponentType()) + "Needs update"};
            ///< Triggered when I need to be updated.
            Event willStartNewTimestep_{std::string(sComponentType()) + "Will start new timestep"};
            ///< Triggered immediately prior to time advancing.
            Event didCompleteTimestep_{std::string(sComponentType()) + "Did complete timestep"};
            ///< Triggered just after fully completing a timestep.
    };
}

#endif // SIM_COMPONENT_DOT_H
