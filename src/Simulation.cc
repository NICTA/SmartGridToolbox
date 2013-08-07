#include "Simulation.h"
#include "Model.h"
#include "Component.h"
#include <algorithm>

namespace SmartGridToolbox
{
   Simulation::Simulation(Model & mod) : mod_(&mod),
                                         startTime_(not_a_date_time),
                                         endTime_(not_a_date_time),
                                         currentTime_(neg_infin)
   {
      // Empty.
   }

   void Simulation::initialize(const ptime & startTime, const ptime & endTime)
   {
      startTime_ = startTime;
      endTime_ = endTime;
      scheduledUpdates_ = ScheduledUpdates();
      contingentUpdates_ = ContingentUpdates();
      for (Component * comp : mod_->getComponents())
      {
         comp->initialize(startTime);
         comp->getEventNeedsUpdate().addAction([this, comp](){contingentUpdates_.insert(comp);},
               "Simulation insert contingent update of component " + comp->getName());
         scheduledUpdates_.insert(comp);
      }
   }

   // TODO: can we tidy up the logic in this function?
   bool Simulation::doNextUpdate()
   {
      bool result = false;
      ptime nextTime = pos_infin;
      Component * schedComp = 0;
      auto schedUpdateIt = scheduledUpdates_.begin();
      if (scheduledUpdates_.size() > 0)
      {
         schedComp = *schedUpdateIt;
         nextTime = schedComp->getValidUntil();
      }
      if (nextTime > currentTime_ && contingentUpdates_.size() > 0 && currentTime_ <= endTime_)
      {
         // There are contingent updates pending.
         Component * contComp = *contingentUpdates_.begin();
         SGT_DEBUG(debug() << "Contingent update component " << contComp->getName() << " from " 
               << schedComp->getTime() << " to " << currentTime_ << std::endl);
         contingentUpdates_.erase(contingentUpdates_.begin()); // Remove from the set.
         // Before updating the component, we need to take it out of the scheduled updates set, because its
         // sort key might change.
         scheduledUpdates_.erase(scheduledUpdates_.find(contComp));
         contComp->update(currentTime_); // Now do the update
         scheduledUpdates_.insert(contComp); // ... and reinsert it in the scheduled updates set.
         result = true;
      }
      else if (scheduledUpdates_.size() > 0 && nextTime <= endTime_)
      {
         // There is a scheduled update to do next.
         currentTime_ = nextTime;
         SGT_DEBUG(debug() << "Scheduled update component " << schedComp->getName() << " from " 
               << schedComp->getTime() << " to " << currentTime_ << std::endl);
         scheduledUpdates_.erase(schedUpdateIt); // Remove the update,
         schedComp->update(currentTime_); // perform the update,
         scheduledUpdates_.insert(schedComp); // and reinsert it.
         result = true;
      }
      return result;
   }
}
