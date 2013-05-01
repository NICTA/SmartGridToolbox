#ifndef SIMULTION_DOT_H
#define SIMULTION_DOT_H

#include "Common.h"
#include "Event.h"
#include <queue>

namespace SmartGridToolbox
{
   class Model;

   /// Simulation. 
   class Simulation
   {
      public:
         /// Default constructor.
         Simulation(Model & mod) : mod_(&mod),
                                   startTime_(not_a_date_time),
                                   endTime_(not_a_date_time)
         {
            // Empty.
         }

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
            return const_cast<Model &>((const_cast<const Simulation *>(this))->
                  getModel());
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

         /// Initialize to start time.
         void initialize(const ptime & startTime, const ptime & endTime);
         
         /// Register an event.
         void addEvent(const Event & event)
         {
            pendingEvents_.push(&event);
         }

         /// Advance timestep.
         void advance();

      private:
         Model * mod_;
         ptime startTime_;
         ptime endTime_;
         std::priority_queue<const Event *> pendingEvents_;
   };
}

#endif // SIMULTION_DOT_H
