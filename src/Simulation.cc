#include "Simulation.h"
#include "Model.h"
#include "Component.h"

namespace SmartGridToolbox
{
   static bool updateComponentComp(const Component * lhs, const Component * rhs)
   {
      return ((lhs->getValidUntil() < rhs->getValidUntil()) ||
              ((lhs->getValidUntil() == rhs->getValidUntil()) &&
               (lhs->getRank() < rhs->getRank())));
   }

   void Simulation::clearQueue()
   {
      pendingUpdates_ = UpdateQueue(updateComponentComp);
   }

   Simulation::Simulation(Model & mod) : mod_(&mod),
                                         startTime_(not_a_date_time),
                                         endTime_(not_a_date_time),
                                         pendingUpdates_(updateComponentComp)
   {
      // Empty.
   }

   void Simulation::initialize(const ptime & startTime, const ptime & endTime)
   {
      startTime_ = startTime;
      endTime_ = endTime;
      clearQueue();
      for (Model::ComponentVec::iterator it = mod_->getComponents().begin();
            it != mod_->getComponents().end(); ++it)
      {
         (**it).initializeComponent(startTime);
         pendingUpdates_.push(*it);
      }
   }

   void Simulation::doNextUpdate()
   {
      Component * comp = pendingUpdates_.top();
      pendingUpdates_.pop();
      comp->update(comp->getValidUntil());
   }
}
