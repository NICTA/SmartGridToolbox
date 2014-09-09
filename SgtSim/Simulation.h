#ifndef SIMULATION_DOT_H
#define SIMULATION_DOT_H

#include <SgtSim/SimComponent.h>
#include <SgtSim/TimeSeries.h>

#include <SgtCore/Common.h>
#include <SgtCore/Event.h>

#include <list>
#include <set>

namespace SmartGridToolbox
{
   /// @brief Simulation: steps time according to discrete event simulation principles.
   /// @ingroup Core
   class Simulation
   {
      private:

         typedef std::shared_ptr<SimComponentAbc> SimCompPtr;
         typedef std::shared_ptr<const SimComponentAbc> SimCompConstPtr;
         typedef std::shared_ptr<TimeSeriesBase> TimeSeriesPtr;
         typedef std::shared_ptr<const TimeSeriesBase> TimeSeriesConstPtr;

         typedef std::vector<SimCompPtr> SimCompVec;
         typedef std::vector<SimCompConstPtr> ConstSimCompVec;
         typedef std::map<std::string, SimCompPtr> SimCompMap;
         typedef std::map<std::string, std::shared_ptr<TimeSeriesBase>> TimeSeriesMap;

      public:
         /// @brief Constructor.
         Simulation();

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

         /// @brief Timezone.
         Time currentTime() const
         {
            return currentTime_;
         }

         /// @brief Factory method for SimComponents.
         template<typename T, typename... Args> std::shared_ptr<T> newSimComponent(Args&&... args)
         {
            std::shared_ptr<T> comp(new T(std::forward<Args>(args)...));
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
            std::shared_ptr<T> comp(new T(std::forward<Args>(args)...));
            replaceSimComponent(comp);
            return comp;
         }

         /// @brief Replace an existing SimComponent with an existing SimComponent.
         template<typename T> void replaceSimComponent(std::shared_ptr<T> comp)
         {
            addOrReplaceGenericSimComponent(comp, true);
         }

         /// @brief Retrieve a const SimComponent.
         template<typename T> std::shared_ptr<const T> simComponent(const std::string& id) const
         {
            SimCompMap::const_iterator it = simCompMap_.find(id);
            return (it == simCompMap_.end()) ? nullptr : std::dynamic_pointer_cast<const T>(it->second);
         }

         /// @brief Retrieve a SimComponent.
         template<typename T> std::shared_ptr<T> simComponent(const std::string& id)
         {
            return std::const_pointer_cast<T>((const_cast<const Simulation*>(this))->simComponent<T>(id));
         }

         /// @brief Copied vector of all const SimComponents.
         ConstSimCompVec simComponents() const;

         /// @brief Copied vector of all SimComponents.
         SimCompVec simComponents() {return simCompVec_;}

         /// @brief Initialize to start time.
         void initialize();

         /// @brief Do the next update.
         bool doNextUpdate();

         /// @brief Complete the pending timestep.
         bool doTimestep();

         /// @brief Get the timestep will start event.
         Event& timestepWillStart() {return timestepWillStart_;}

         /// @brief Get the timestep did complete event.
         Event& timestepDidComplete() {return timestepDidComplete_;}

         /// @brief Get named TimeSeries.
         template<typename T> std::shared_ptr<const T> timeSeries(const std::string& id) const
         {
            TimeSeriesMap::const_iterator it = timeSeriesMap_.find(id);
            return (it == timeSeriesMap_.end()) ? 0 : std::const_pointer_cast<const T>(it->second);
         }

         /// @brief Get time series with given id.
         template<typename T> std::shared_ptr<T> timeSeries(const std::string& id)
         {
            return std::const_pointer_cast<T>((const_cast<const Simulation*>(this))->timeSeries<T>(id));
         }

         /// @brief Add a time series.
         void acquireTimeSeries (const std::string& id, std::shared_ptr<TimeSeriesBase> timeSeries)
         {
            timeSeriesMap_[id] = std::move(timeSeries);
         }

         /// @brief Make ready to simulate!
         void validate();

      private:

         /// @brief Soonest/smallest rank goes first. If equal rank and time, then sort on the id.
         class ScheduledUpdatesCompare
         {
            public:
               bool operator()(const std::pair<SimCompPtr, Time>& lhs,
                     const std::pair<SimCompPtr, Time>& rhs)
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
               bool operator()(const SimCompPtr lhs, const SimCompPtr rhs)
               {
                  return ((lhs->rank() < rhs->rank()) ||
                        ((lhs->rank() == rhs->rank()) && (lhs->id() < rhs->id())));
               }
         };

         typedef std::set<std::pair<SimCompPtr, Time>, ScheduledUpdatesCompare> ScheduledUpdates;
         typedef std::set<SimCompPtr, ContingentUpdatesCompare> ContingentUpdates;

      private:

         void addOrReplaceGenericSimComponent(std::shared_ptr<SimComponentAbc> simComp, bool allowReplace);
      
      private:

         bool isValid_;
         
         Time startTime_;
         Time endTime_;
         LatLong latLong_;
         local_time::time_zone_ptr timezone_;
         
         SimCompMap simCompMap_;
         SimCompVec simCompVec_; // Encoding order of evaluation/rank.

         TimeSeriesMap timeSeriesMap_; // Contains TimeSeries objects for Simulation.

         Time currentTime_;
         ScheduledUpdates scheduledUpdates_;
         ContingentUpdates contingentUpdates_;
         Event timestepWillStart_;
         Event timestepDidComplete_;
   };
}

#endif // SIMULATION_DOT_H
