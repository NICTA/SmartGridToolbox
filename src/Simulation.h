#ifndef SIMULTION_DOT_H
#define SIMULTION_DOT_H

#include "Common.h"
#include "Component.h"
#include "Event.h"
#include <list>
#include <set>

namespace SmartGridToolbox
{
   class Model;

   // Soonest/smallest rank goes first. If equal rank and time, then sort on the name.
   class ScheduledUpdatesComp
   {
      public:
         bool operator()(const Component * lhs, const Component * rhs)
         {
            return ((lhs->validUntil() < rhs->validUntil()) ||
                    (lhs->validUntil() == rhs->validUntil() && lhs->rank() < rhs->rank()) ||
                    (lhs->validUntil() == rhs->validUntil() && lhs->rank() == rhs->rank() && 
                     (lhs->name() < rhs->name())));
         }
   };

   // Smallest rank goes first. If equal rank, then sort on the name.
   class ContingentUpdatesComp
   {
      public:
         bool operator()(const Component * lhs, const Component * rhs)
         {
            return ((lhs->rank() < rhs->rank()) ||
                    ((lhs->rank() == rhs->rank()) && (lhs->name() < rhs->name())));
         }
   };

   /// Simulation. 
   class Simulation
   {
      public:
         /// Default constructor.
         Simulation(Model & mod);

         /// Destructor.
         ~Simulation()
         {
            // Empty.
         }

         /// Model accessor.
         const Model & model() const
         {
            return *mod_;
         }

         /// Model accessor.
         Model & model()
         {
            return const_cast<Model &>((const_cast<const Simulation *>(this))->model());
         }

         const ptime & startTime() const
         {
            return startTime_;
         }

         void setStartTime(const ptime time)
         {
            startTime_ = time;
         }

         const ptime & endTime() const
         {
            return endTime_;
         }

         void setEndTime(const ptime time)
         {
            endTime_ = time;
         }

         const ptime & currentTime() const
         {
            return currentTime_;
         }

         /// Initialize to start time.
         void initialize();
         
         /// Do the next update.
         bool doNextUpdate();

         /// Signal that an abnormal exit should occur.
         void signalAbnormalExit();

         /// Get the time did advance event.
         Event & eventTimeDidAdvance() {return timeDidAdvance_;}

      private:
         typedef std::set<Component *, ScheduledUpdatesComp> ScheduledUpdates;
         typedef std::set<Component *, ContingentUpdatesComp> ContingentUpdates;

      private:
         Model * mod_;
         ptime startTime_;
         ptime endTime_;
         ptime currentTime_;
         ScheduledUpdates scheduledUpdates_;
         ContingentUpdates contingentUpdates_;
         Event timeDidAdvance_;
   };
}

#endif // SIMULTION_DOT_H
