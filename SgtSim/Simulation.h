#ifndef SIMULATION_DOT_H
#define SIMULATION_DOT_H

#include <SgtSim/Event.h>
#include <SgtSim/SimObject.h>
#include <SgtSim/TimeSeries.h>

#include <SgtCore/Common.h>

#include <list>
#include <set>

namespace SmartGridToolbox
{
   /// @brief Simulation: steps time according to discrete event simulation principles.
   /// @ingroup Core
   class Simulation
   {
      private:

         typedef std::shared_ptr<SimObject> SimObjPtr;
         typedef std::shared_ptr<const SimObject> SimObjConstPtr;
         typedef std::shared_ptr<TimeSeriesBase> TimeSeriesPtr;
         typedef std::shared_ptr<const TimeSeriesBase> TimeSeriesConstPtr;

         typedef std::vector<SimObjPtr> SimObjVec;
         typedef std::vector<SimObjConstPtr> ConstSimObjVec;
         typedef std::map<std::string, SimObjPtr> SimObjMap;
         typedef std::map<std::string, std::unique_ptr<TimeSeriesBase>> TimeSeriesMap;

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

         /// @brief Factory method for SimObjects.
         template<typename T, typename... Args> T& newSimObject(Args&&... args)
         {
            std::shared_ptr<T> comp(new T(std::forward<Args>(args)...));
            return acquireSimObject(comp);
         }

         /// @brief Acquire an existing SimObject.
         template<typename T> T& acquireSimObject(std::shared_ptr<T> comp)
         {
            addOrReplaceGenericSimObject(comp, false);
            return *comp;
         }

         /// @brief Replace an existing SimObject factory method.
         template<typename T, typename... Args> T& replaceSimObjectWithNew(Args&&... args)
         {
            std::shared_ptr<T> comp(new T(std::forward<Args>(args)...));
            return replaceSimObject(comp);
         }

         /// @brief Replace an existing SimObject with an existing SimObject.
         template<typename T> T& replaceSimObject(std::shared_ptr<T> comp)
         {
            addOrReplaceGenericSimObject(comp, true);
            return *comp;
         }

         /// @brief Retrieve a const SimObject.
         template<typename T> std::shared_ptr<T> simObject(const std::string& id) const
         {
            SimObjMap::const_iterator it = simObjMap_.find(id);
            return (it == simObjMap_.end()) ? nullptr : std::dynamic_pointer_cast<const T>(it->second);
         }

         /// @brief Retrieve a SimObject.
         template<typename T> std::shared_ptr<T> simObject(const std::string& id)
         {
            return std::const_pointer_cast<std::shared_ptr<T>>((const_cast<const Simulation*>(this))->simObject<T>(id));
         }

         /// @brief Copied vector of all const SimObjects.
         ConstSimObjVec simObjects() const;

         /// @brief Copied vector of all SimObjects.
         SimObjVec simObjects() {return simObjVec_;}

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
            return (it == timeSeriesMap_.end()) ? 0 : std::const_pointer_cast<std::shared_ptr<const T>>(it->second);
         }

         /// @brief Get time series with given id.
         template<typename T> std::shared_ptr<T> timeSeries(const std::string& id)
         {
            return std::const_pointer_cast<T>((const_cast<const Simulation*>(this))-> timeSeries<T>(id));
         }

         /// @brief Add a time series.
         void acquireTimeSeries (const std::string& id, std::unique_ptr<TimeSeriesBase> timeSeries)
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
               bool operator()(const std::pair<SimObjPtr, Time>& lhs,
                     const std::pair<SimObjPtr, Time>& rhs)
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
               bool operator()(const SimObjPtr lhs, const SimObjPtr rhs)
               {
                  return ((lhs->rank() < rhs->rank()) ||
                        ((lhs->rank() == rhs->rank()) && (lhs->id() < rhs->id())));
               }
         };

         typedef std::set<std::pair<SimObjPtr, Time>, ScheduledUpdatesCompare> ScheduledUpdates;
         typedef std::set<SimObjPtr, ContingentUpdatesCompare> ContingentUpdates;

      private:

         void addOrReplaceGeneric(std::shared_ptr<SimObject> simObj, bool allowReplace);
      
      private:

         bool isValid_;
         
         Time startTime_;
         Time endTime_;
         LatLong latLong_;
         local_time::time_zone_ptr timezone_;
         
         SimObjMap simObjMap_;
         SimObjVec simObjVec_; // Encoding order of evaluation/rank.

         TimeSeriesMap timeSeriesMap_; // Contains TimeSeries objects for Simulation.

         Time currentTime_;
         ScheduledUpdates scheduledUpdates_;
         ContingentUpdates contingentUpdates_;
         Event timestepWillStart_;
         Event timestepDidComplete_;
   };
}

#endif // SIMULATION_DOT_H
