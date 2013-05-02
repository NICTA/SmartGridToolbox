#ifndef SIMULTION_DOT_H
#define SIMULTION_DOT_H

#include "Common.h"
#include "Component.h"
#include <queue>

namespace SmartGridToolbox
{
   class Model;

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
         
         /// Do the next update.
         void doNextUpdate();

      private:
         typedef std::priority_queue<Component *, std::vector<Component *>,
                 bool(*)(const Component *, const Component *)> UpdateQueue;

      private:
         void clearQueue();

      private:
         Model * mod_;
         ptime startTime_;
         ptime endTime_;
         UpdateQueue pendingUpdates_;
   };
}

#endif // SIMULTION_DOT_H
