#include "Simulation.h"
#include "Model.h"
#include "Component.h"
#include <algorithm>

namespace SmartGridToolbox
{
   Simulation::Simulation(Model & mod) : mod_(&mod),
                                         startTime_(not_a_date_time),
                                         endTime_(not_a_date_time)
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

   void Simulation::doNextUpdate()
   {
      // Check the next scheduled update.
      auto schedUpdateIt = scheduledUpdates_.begin();
      Component * schedComp = *schedUpdateIt;
      ptime nextTime = schedComp->getValidUntil();
      if (nextTime > latestTime_ && contingentUpdates_.size() > 0)
      {
         // There are still contingent updates that need to be cleared before time is advanced.
         Component * contComp = *contingentUpdates_.begin();
         SGTDebug("Contingent update component " << contComp->getName() << " from " 
               << schedComp->getTime() << " to " << latestTime_);
         contingentUpdates_.erase(contingentUpdates_.begin()); // Remove from the set.
         // Before updating the component, we need to take it out of the scheduled updates set, because its
         // sort key might change.
         scheduledUpdates_.erase(scheduledUpdates_.find(contComp));
         contComp->update(latestTime_); // Now do the update
         scheduledUpdates_.insert(contComp); // ... and reinsert it in the scheduled updates set.
      }
      else
      {
         // Do the next scheduled update.
         SGTDebug("Scheduled update component " << schedComp->getName() << " from " 
               << schedComp->getTime() << " to " << schedComp->getValidUntil());
         scheduledUpdates_.erase(schedUpdateIt); // Remove the update,
         schedComp->update(schedComp->getValidUntil()); // perform the update,
         scheduledUpdates_.insert(schedComp); // and reinsert it.
      }
   }
}
