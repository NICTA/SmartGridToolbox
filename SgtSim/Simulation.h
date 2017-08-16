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

#ifndef SIMULATION_DOT_H
#define SIMULATION_DOT_H

#include <SgtSim/SimComponent.h>
#include <SgtSim/TimeSeries.h>

#include <SgtCore/Common.h>
#include <SgtCore/ComponentCollection.h>
#include <SgtCore/Event.h>

#include <list>
#include <set>

namespace Sgt
{
    /// @brief Simulation: steps time according to discrete event simulation principles.
    /// @ingroup SimCore
    class Simulation
    {
        public:

            /// @brief Constructor.
            Simulation() = default;

            /// @brief Simulation start time.
            Time startTime() const
            {
                return startTime_;
            }

            /// @brief Simulation start time.
            void setStartTime(Time time)
            {
                startTime_ = time;
            }

            /// @brief Simulation end time.
            Time endTime() const
            {
                return endTime_;
            }

            /// @brief Simulation end time.
            void setEndTime(Time time)
            {
                endTime_ = time;
            }

            /// @brief Latitude/Longitude.
            LatLong latLong() const
            {
                return latLong_;
            }

            /// @brief Latitude/Longitude.
            void setLatLong(const LatLong& latLong)
            {
                latLong_ = latLong;
            }

            /// @brief Timezone.
            Timezone timezone() const
            {
                return timezone_;
            }

            /// @brief Timezone.
            void setTimezone(Timezone tz)
            {
                timezone_ = tz;
            }

            /// @brief Current time.
            Time currentTime() const
            {
                return currentTime_;
            }

            /// @brief Access SimComponents (const version).
            const ComponentCollection<SimComponent>& simComponents() const
            {
                return simComps_;
            }
            
            /// @brief Access SimComponents (non-const version).
            ComponentCollection<SimComponent>& simComponents()
            {
                return simComps_;
            }
            
            /// @brief Access a particular SimComponent of a given type.
            template<typename T> ConstSimComponentPtr<T> simComponent(const std::string& id) const
            {
                return simComps_[id];
            }
            
            /// @brief Access a particular SimComponent of a given type.
            template<typename T> SimComponentPtr<T> simComponent(const std::string& id)
            {
                return simComps_[id].as<T>();
            }
            
            /// @brief Add a SimComponent.
            ComponentPtr<SimComponent> addSimComponent(std::shared_ptr<SimComponent> comp)
            {
                return simComps_.insert(comp->id(), comp);
            }
            
            /// @brief Factory method for SimComponents.
            template<typename T, typename... Args> SimComponentPtr<T> newSimComponent(Args&&... args)
            {
                auto comp = std::make_shared<T>(std::forward<Args>(args)...);
                return simComps_.insert(comp->id(), comp).template as<T>();
            }
            
            /// @brief Remove a SimComponent.
            std::shared_ptr<SimComponent> removeSimComponent(const std::string& id)
            {
                return simComps_.remove(id);
            }

            /// @brief Initialize to start time.
            void initialize();

            /// @brief Do the next update.
            void doNextUpdate();

            /// @brief Complete the pending timestep.
            void doTimestep();

            /// @brief Print components and their dependencies to log.
            void logComponents() const;

            /// @brief Have we reached the end of the simulation?
            bool isFinished() const {return (scheduledUpdates_.size() == 0) && (contingentUpdates_.size() == 0);}

            /// @brief Get the timestep will start event.
            const Event& timestepWillStart() const {return timestepWillStart_;}

            /// @brief Get the timestep did complete event.
            const Event& timestepDidComplete() const {return timestepDidComplete_;}

            /// @brief Access TimeSeries (const version).
            const ComponentCollection<TimeSeriesBase>& timeSeries() const
            {
                return timeSeries_;
            }
            
            /// @brief Access TimeSeries (non-const version).
            ComponentCollection<TimeSeriesBase>& timeSeries()
            {
                return timeSeries_;
            }
            
            /// @brief Add a TimeSeries.
            ComponentPtr<TimeSeriesBase> addTimeSeries(const std::string& id, std::shared_ptr<TimeSeriesBase> ts)
            {
                return timeSeries_.insert(id, ts);
            }
            
            /// @brief Remove a TimeSeries.
            std::shared_ptr<TimeSeriesBase> removeTimeSeries(const std::string& id)
            {
                return timeSeries_.remove(id);
            }

        private:

            /// @brief Soonest/smallest rank goes first. If equal rank and time, then sort on the id.
            class ScheduledUpdatesCompare
            {
                public:
                    bool operator()(const std::pair<ComponentPtr<SimComponent>, Time>& lhs,
                                    const std::pair<ComponentPtr<SimComponent>, Time>& rhs)
                    {
                        return ((lhs.second < rhs.second) ||
                                (lhs.second == rhs.second && lhs.first->rank() < rhs.first->rank()) ||
                                (lhs.second == rhs.second && lhs.first->rank() == rhs.first->rank() &&
                                 (lhs.first->id() < rhs.first->id())));
                    }
            };

            // Smallest rank goes first. If equal rank, then sort on the id.
            class ContingentUpdatesCompare
            {
                public:
                    bool operator()(const ComponentPtr<SimComponent>& lhs, const ComponentPtr<SimComponent>& rhs)
                    {
                        return ((lhs->rank() < rhs->rank()) ||
                                ((lhs->rank() == rhs->rank()) && (lhs->id() < rhs->id())));
                    }
            };

            using ScheduledUpdate = std::pair<ComponentPtr<SimComponent>, Time>;
            using ScheduledUpdates = std::set<ScheduledUpdate, ScheduledUpdatesCompare>;
            using ContingentUpdates = std::set<ComponentPtr<SimComponent>, ContingentUpdatesCompare>;

        private:

            void tryInsertScheduledUpdate(const ComponentPtr<SimComponent>& schedComp);

        private:

            bool isValid_ = false;

            Time startTime_{TimeSpecialValues::not_a_date_time};
            Time endTime_{TimeSpecialValues::not_a_date_time};
            LatLong latLong_;
            Timezone timezone_;

            MutableComponentCollection<SimComponent> simComps_;
            MutableComponentCollection<TimeSeriesBase> timeSeries_;

            Time currentTime_{TimeSpecialValues::neg_infin};
            ScheduledUpdates scheduledUpdates_;
            ContingentUpdates contingentUpdates_;
            Event timestepWillStart_{"Simulation timestep will start"};
            Event timestepDidComplete_{"Simulation timestep did complete"};
    };
}

#endif // SIMULATION_DOT_H
