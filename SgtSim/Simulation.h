#ifndef SIMULATION_DOT_H
#define SIMULATION_DOT_H

#include <SgtSim/Event.h>
#include <SgtSim/TimeSeries.h>
#include <SgtSim/SimComponent.h>

#include <SgtCore/Common.h>

#include <list>
#include <set>

namespace SmartGridToolbox
{
   class Model;

   /// @brief Simulation: takes a Model, and steps time according to discrete event simulation principles.
   /// @ingroup Core
   class Simulation
   {
      private:

         typedef std::map<std::string, std::unique_ptr<TimeSeriesBase>> TimeSeriesMap;
         typedef std::vector<SimComponent*> ComponentVec;

      public:
         /// @brief Default constructor.
         Simulation(Model& mod);

         /// @brief Destructor.
         ~Simulation()
         {
            // Empty.
         }

         /// @brief Model accessor.
         const Model& model() const
         {
            return *mod_;
         }

         /// @brief Model accessor.
         Model& model()
         {
            return const_cast<Model &>((const_cast<const Simulation*>(this))->model());
         }

         Time startTime() const
         {
            return startTime_;
         }

         void setStartTime(Time time)
         {
            startTime_ = time;
         }

         Time endTime() const
         {
            return endTime_;
         }

         void setEndTime(Time time)
         {
            endTime_ = time;
         }

         LatLong latLong() const
         {
            return latLong_;
         }

         void setLatLong(const LatLong& latLong)
         {
            latLong_ = latLong;
         }

         const local_time::time_zone_ptr timezone() const
         {
            return timezone_;
         }

         void setTimezone(local_time::time_zone_ptr tz)
         {
            timezone_ = tz;
         }

         Time currentTime() const
         {
            return currentTime_;
         }

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

         template<typename T> const T* timeSeries(const std::string& name) const
         {
            TimeSeriesMap::const_iterator it = timeSeriesMap_.find(name);
            return (it == timeSeriesMap_.end()) ? 0 : dynamic_cast<const T*>(it->second.get());
         }
         template<typename T> T* timeSeries(const std::string& name)
         {
            return const_cast<T*>((const_cast<const Simulation*>(this))-> timeSeries<T>(name));
         }

         void acquireTimeSeries (const std::string& name, std::unique_ptr<TimeSeriesBase> timeSeries)
         {
            timeSeriesMap_[name] = std::move(timeSeries);
         }

         void validate();

      private:

         /// @brief Soonest/smallest rank goes first. If equal rank and time, then sort on the name.
         class ScheduledUpdatesCompare
         {
            public:
               bool operator()(const std::pair<SimComponent*, Time>& lhs,
                     const std::pair<SimComponent*, Time>& rhs)
               {
                  return ((lhs.second < rhs.second) ||
                        (lhs.second == rhs.second && lhs.first->rank() < rhs.first->rank()) ||
                        (lhs.second == rhs.second && lhs.first->rank() == rhs.first->rank() &&
                         (lhs.first->name() < rhs.first->name())));
               }
         };

         // Smallest rank goes first. If equal rank, then sort on the name.
         class ContingentUpdatesCompare
         {
            public:
               bool operator()(const SimComponent* lhs, const SimComponent* rhs)
               {
                  return ((lhs->rank() < rhs->rank()) ||
                        ((lhs->rank() == rhs->rank()) && (lhs->name() < rhs->name())));
               }
         };

         typedef std::set<std::pair<SimComponent*, Time>, ScheduledUpdatesCompare> ScheduledUpdates;
         typedef std::set<SimComponent*, ContingentUpdatesCompare> ContingentUpdates;

      private:

         bool isValid_;
         
         Model* mod_;

         Time startTime_;
         Time endTime_;
         LatLong latLong_;
         local_time::time_zone_ptr timezone_;
         
         SimComponentVec compVec_; // Encoding order of evaluation/rank.
         TimeSeriesMap timeSeriesMap_; // Contains TimeSeries objects for Simulation.

         Time currentTime_;
         ScheduledUpdates scheduledUpdates_;
         ContingentUpdates contingentUpdates_;
         Event timestepWillStart_;
         Event timestepDidComplete_;
   };
}

#endif // SIMULATION_DOT_H
