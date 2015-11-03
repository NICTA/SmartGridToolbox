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
            const local_time::time_zone_ptr timezone() const
            {
                return timezone_;
            }

            /// @brief Timezone.
            void setTimezone(local_time::time_zone_ptr tz)
            {
                timezone_ = tz;
            }

            /// @brief Current time.
            Time currentTime() const
            {
                return currentTime_;
            }

            /// @brief Factory method for SimComponents.
            template<typename T, typename... Args> std::shared_ptr<T> newSimComponent(Args&&... args)
            {
                auto comp = std::make_shared<T>(std::forward<Args>(args)...);
                acquireSimComponent(comp);
                return comp;
            }

            /// @brief Acquire an existing SimComponent.
            template<typename T> void acquireSimComponent(std::shared_ptr<T> comp)
            {
                addOrReplaceGenericSimComponent(comp, false);
            }

            /// @brief Replace an existing SimComponent factory method.
            template<typename T, typename... Args> std::shared_ptr<T> replaceSimComponentWithNew(Args&&... args)
            {
                auto comp = std::make_shared<T>(new T(std::forward<Args>(args)...));
                replaceSimComponent(comp);
                return comp.get();
            }

            /// @brief Replace an existing SimComponent with an existing SimComponent.
            template<typename T> void replaceSimComponent(std::shared_ptr<T> comp)
            {
                addOrReplaceGenericSimComponent(comp, true);
            }

            /// @brief Retrieve a const SimComponent.
            template<typename T> const T* simComponent(const std::string& id, bool crashOnFail = true) const
            {
                const SimComponent* simComp = genericSimComponent(id, crashOnFail);
                auto result = dynamic_cast<const T*>(simComp);
                sgtAssert(!(result == nullptr && crashOnFail), "Component " << id 
                        << " was requested and exists in the simulation, but is of the wrong type.");
                return result;
            }

            /// @brief Retrieve a SimComponent.
            template<typename T> T* simComponent(const std::string& id, bool crashOnFail = true)
            {
                return const_cast<T*>((static_cast<const Simulation*>(this))->simComponent<T>(id, crashOnFail));
            }

            /// @brief Copied vector of all const SimComponents.
            std::vector<const SimComponent*> simComponents() const;

            /// @brief Copied vector of all SimComponents.
            std::vector<SimComponent*> simComponents() {return simCompVec_;}

            /// @brief Initialize to start time.
            void initialize();

            /// @brief Do the next update.
            void doNextUpdate();

            /// @brief Complete the pending timestep.
            void doTimestep();

            /// @brief Print components and their dependencies to log.
            void logComponents();

            /// @brief Have we reached the end of the simulation?
            bool isFinished() {return (scheduledUpdates_.size() == 0) && (contingentUpdates_.size() == 0);}

            /// @brief Get the timestep will start event.
            Event& timestepWillStart() {return timestepWillStart_;}

            /// @brief Get the timestep did complete event.
            Event& timestepDidComplete() {return timestepDidComplete_;}

            /// @brief Retrieve a const TimeSeries.
            template<typename T> const T* timeSeries(const std::string& id, bool crashOnFail = true) const
            {
                const TimeSeriesBase* ts = genericTimeSeries(id, crashOnFail);
                auto result = dynamic_cast<const T*>(ts);
                sgtAssert(!(result == nullptr && crashOnFail), "Time series " << id
                        << " was requested and exists in the simulation, but is of the wrong type.");
                return result;
            }

            /// @brief Retrieve a TimeSeries.
            template<typename T> T* timeSeries(const std::string& id, bool crashOnFail = true)
            {
                return const_cast<T*>((static_cast<const Simulation*>(this))->timeSeries<T>(id, crashOnFail));
            }

            /// @brief Add a time series.
            void acquireTimeSeries (const std::string& id, std::shared_ptr<TimeSeriesBase> timeSeries)
            {
                timeSeriesMap_[id] = timeSeries;
            }

        private:

            /// @brief Soonest/smallest rank goes first. If equal rank and time, then sort on the id.
            class ScheduledUpdatesCompare
            {
                public:
                    bool operator()(const std::pair<SimComponent*, Time>& lhs,
                                    const std::pair<SimComponent*, Time>& rhs)
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
                    bool operator()(const SimComponent* lhs, const SimComponent* rhs)
                    {
                        return ((lhs->rank() < rhs->rank()) ||
                                ((lhs->rank() == rhs->rank()) && (lhs->id() < rhs->id())));
                    }
            };

            typedef std::pair<SimComponent*, Time> ScheduledUpdate;
            typedef std::set<ScheduledUpdate, ScheduledUpdatesCompare> ScheduledUpdates;
            typedef std::set<SimComponent*, ContingentUpdatesCompare> ContingentUpdates;

        private:

            const SimComponent* genericSimComponent(const std::string& id, bool crashOnFail = true) const;

            void addOrReplaceGenericSimComponent(std::shared_ptr<SimComponent> simComp, bool allowReplace);

            const TimeSeriesBase* genericTimeSeries(const std::string& id, bool crashOnFail = true) const;

            void tryInsertScheduledUpdate(SimComponent* schedComp);

        private:

            bool isValid_ = false;

            Time startTime_{posix_time::not_a_date_time};
            Time endTime_{posix_time::not_a_date_time};
            LatLong latLong_;
            local_time::time_zone_ptr timezone_;

            std::map<std::string, std::shared_ptr<SimComponent>> simCompMap_;
            std::vector<SimComponent*> simCompVec_; // Encoding order of evaluation/rank.

            std::map<std::string, std::shared_ptr<TimeSeriesBase>> timeSeriesMap_;

            Time currentTime_{posix_time::neg_infin};
            ScheduledUpdates scheduledUpdates_;
            ContingentUpdates contingentUpdates_;
            Event timestepWillStart_{"Simulation timestep will start"};
            Event timestepDidComplete_{"Simulation timestep did complete"};
    };
}

#endif // SIMULATION_DOT_H
