#ifndef SIMULTION_DOT_H
#define SIMULTION_DOT_H

#include <SmartGridToolbox/Common.h>
#include <SmartGridToolbox/Component.h>
#include <SmartGridToolbox/Event.h>
#include <list>
#include <set>

namespace SmartGridToolbox
{
   class Model;

   // Soonest/smallest rank goes first. If equal rank and time, then sort on the name.
   class ScheduledUpdatesComp
   {
      public:
         bool operator()(const std::pair<Component *, Time> & lhs, 
                         const std::pair<Component *, Time> & rhs) 
         {
            return ((lhs.second < rhs.second) ||
                    (lhs.second == rhs.second && lhs.first->rank() < rhs.first->rank()) ||
                    (lhs.second == rhs.second && lhs.first->rank() == rhs.first->rank() && 
                     (lhs.first->name() < rhs.first->name())));
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

         Time currentTime() const
         {
            return currentTime_;
         }

         /// Initialize to start time.
         void initialize();
         
         /// Do the next update.
         bool doNextUpdate();

         /// Complete the pending timestep.
         bool doTimestep();

         /// Signal that an abnormal exit should occur.
         void signalAbnormalExit();

         /// Get the timestep will start event.
         Event & timestepWillStart() {return timestepWillStart_;}

         /// Get the timestep did complete event.
         Event & timestepDidComplete() {return timestepDidComplete_;}

      private:
         typedef std::set<std::pair<Component *, Time>, ScheduledUpdatesComp> ScheduledUpdates;
         typedef std::set<Component *, ContingentUpdatesComp> ContingentUpdates;

      private:
         Model * mod_;
         Time startTime_;
         Time endTime_;
         Time currentTime_;
         ScheduledUpdates scheduledUpdates_;
         ContingentUpdates contingentUpdates_;
         Event timestepWillStart_;
         Event timestepDidComplete_;
   };
}

#endif // SIMULTION_DOT_H
