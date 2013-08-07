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
            return ((lhs->getValidUntil() < rhs->getValidUntil()) ||
                    (lhs->getValidUntil() == rhs->getValidUntil() && lhs->getRank() < rhs->getRank()) ||
                    (lhs->getValidUntil() == rhs->getValidUntil() && lhs->getRank() == rhs->getRank() && 
                     (lhs->getName() < rhs->getName())));
         }
   };

   // Smallest rank goes first. If equal rank, then sort on the name.
   class ContingentUpdatesComp
   {
      public:
         bool operator()(const Component * lhs, const Component * rhs)
         {
            return ((lhs->getRank() < rhs->getRank()) ||
                    ((lhs->getRank() == rhs->getRank()) && (lhs->getName() < rhs->getName())));
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
         const Model & getModel() const
         {
            return *mod_;
         }

         /// Model accessor.
         Model & getModel()
         {
            return const_cast<Model &>((const_cast<const Simulation *>(this))->getModel());
         }

         const ptime & getStartTime() const
         {
            return startTime_;
         }

         void setStartTime(const ptime time)
         {
            startTime_ = time;
         }

         const ptime & getEndTime() const
         {
            return endTime_;
         }

         void setEndTime(const ptime time)
         {
            endTime_ = time;
         }

         const ptime & getCurrentTime() const
         {
            return currentTime_;
         }

         /// Initialize to start time.
         void initialize(const ptime & startTime, const ptime & endTime);
         
         /// Do the next update.
         bool doNextUpdate();

         /// Get the time did advance event.
         Event & getEventTimeDidAdvance() {return timeDidAdvance_;}

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
