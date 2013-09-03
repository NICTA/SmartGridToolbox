#include "Simulation.h"
#include "Model.h"
#include "Component.h"
#include <algorithm>

namespace SmartGridToolbox
{
   Simulation::Simulation(Model & mod) : mod_(&mod),
                                         startTime_(not_a_date_time),
                                         endTime_(not_a_date_time),
                                         currentTime_(neg_infin),
                                         timeDidAdvance_("Simulation time did advance")
   {
      // Empty.
   }

   void Simulation::initialize()
   {
      scheduledUpdates_ = ScheduledUpdates();
      contingentUpdates_ = ContingentUpdates();
      for (Component * comp : mod_->components())
      {
         comp->initialize(startTime_);
         comp->eventNeedsUpdate().addAction([this, comp](){contingentUpdates_.insert(comp);},
               "Simulation insert contingent update of component " + comp->name());
         scheduledUpdates_.insert(comp);
      }
      currentTime_ = startTime_;
   }

   // TODO: can we tidy up the logic in this function?
   bool Simulation::doNextUpdate()
   {
      SGT_DEBUG(debug() << "Simulation doNextUpdate(): " << std::endl);
      SGT_DEBUG(debug() << "\tNumber of scheduled = " << scheduledUpdates_.size() << std::endl);
      SGT_DEBUG(debug() << "\tNumber of contingent = " << scheduledUpdates_.size() << std::endl);
      bool result = false;
      bool didAdvance = false;
      Time nextSchedTime = pos_infin;
      Component * schedComp = 0;
      auto schedUpdateIt = scheduledUpdates_.begin();
      if (scheduledUpdates_.size() > 0)
      {
         schedComp = *schedUpdateIt;
         nextSchedTime = schedComp->validUntil();
         SGT_DEBUG(debug() << "\tStart time = " << startTime_ << std::endl);
         SGT_DEBUG(debug() << "\tCurrent time = " << currentTime_ << std::endl);
         SGT_DEBUG(debug() << "\tNext scheduled time = " << nextSchedTime << std::endl);
         SGT_DEBUG(debug() << "\tEnd time = " << endTime_ << std::endl);
      }
      if (nextSchedTime > currentTime_ && contingentUpdates_.size() > 0 && currentTime_ <= endTime_)
      {
         // There are contingent updates pending.
         Component * contComp = *contingentUpdates_.begin();
         SGT_DEBUG(debug() << "\tContingent update component " << contComp->name() << " from " 
               << schedComp->time() << " to " << currentTime_ << std::endl);
         contingentUpdates_.erase(contingentUpdates_.begin()); // Remove from the set.
         // Before updating the component, we need to take it out of the scheduled updates set, because its
         // sort key might change.
         scheduledUpdates_.erase(scheduledUpdates_.find(contComp));
         contComp->update(currentTime_); // Now do the update
         scheduledUpdates_.insert(contComp); // ... and reinsert it in the scheduled updates set.
         result = true;
      }
      else if (scheduledUpdates_.size() > 0 && nextSchedTime <= endTime_)
      {
         // There is a scheduled update to do next.
         if (nextSchedTime > currentTime_)
         {
            didAdvance = true;
         }
         currentTime_ = nextSchedTime;
         SGT_DEBUG(debug() << "\tScheduled update component " << schedComp->name() << " from " 
               << schedComp->time() << " to " << currentTime_ << std::endl);
         scheduledUpdates_.erase(schedUpdateIt); // Remove the update,
         schedComp->update(currentTime_); // perform the update,
         scheduledUpdates_.insert(schedComp); // and reinsert it.
         result = true;
      }
      else
      {
         SGT_DEBUG(debug() << "\tNo update." << std::endl); 
      }
      if (didAdvance)
      {
         timeDidAdvance_.trigger();
      }
      return result;
   }
}
