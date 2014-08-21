#include "Simulation.h"

#include <algorithm>

#define DEBUG 1

namespace SmartGridToolbox
{
   Simulation::Simulation() : 
      startTime_(posix_time::not_a_date_time),
      endTime_(posix_time::not_a_date_time),
      currentTime_(posix_time::neg_infin),
      timestepWillStart_("Simulation timestep will start"),
      timestepDidComplete_("Simulation timestep did complete")
   {
      // Empty.
   }
   
   Simulation::ConstSimObjVec Simulation::simObjects() const
   {
      ConstSimObjVec result(simObjVec_.size());
      std::copy(simObjVec_.begin(), simObjVec_.end(), result.begin());
      return result;
   }

   void Simulation::addOrReplaceGeneric(std::unique_ptr<SimObject> && simObj, bool allowReplace)
   {
      SimObject& ref = *simObj;

      message() << "Adding simObject " << ref.id() << " of type " 
         << ref.componentTypeStr() << " to model." << std::endl;
      IndentingOStreamBuf _(messageStream());

      SimObjMap::iterator it1 = simObjMap_.find(ref.id());
      if (it1 != simObjMap_.end())
      {
         if (allowReplace)
         {
            it1->second = std::move(comp);
            message() << "SimObject " << ref.id() << " replaced in model." << std::endl;
         }
         else
         {
            error() << "SimObject " << ref.id() << " occurs more than once in the model!" << std::endl;
            abort();
         }
      }
      else
      {
         simObjVec_.push_back(comp.get());
         simObjMap_[ref.id()] = std::move(comp);
      }
   }

   void Simulation::initialize()
   {
      scheduledUpdates_.clear();
      for (SimObject* comp : mod_->simObjects())
      {
         comp->initialize();
         scheduledUpdates_.insert(std::make_pair(comp, startTime_));
         comp->needsUpdate().addAction([this, comp](){contingentUpdates_.insert(comp);},
                                       "Simulation insert contingent update of simObject " + comp->name());
      }
      currentTime_ = posix_time::neg_infin;
      // Contingent updates may have been inserted during initialization process e.g. when setting up setpoints etc.
      contingentUpdates_.clear();
      doTimestep(); // Do the first timestep, to advance the start time for -infinity to startTime_.
   }

   // TODO: can we tidy up the logic in this function?
   bool Simulation::doNextUpdate()
   {
      SGT_DEBUG(debug() << "Simulation doNextUpdate(): " << std::endl);
      SGT_DEBUG(debug() << "\tNumber of scheduled = " << scheduledUpdates_.size() << std::endl);
      SGT_DEBUG(debug() << "\tNumber of contingent = " << contingentUpdates_.size() << std::endl);

      bool result = false;

      Time nextSchedTime = posix_time::pos_infin;
      SimObject* schedComp = 0;
      auto schedUpdateIt = scheduledUpdates_.begin();

      if (scheduledUpdates_.size() > 0)
      {
         schedComp = schedUpdateIt->first;
         nextSchedTime = schedUpdateIt->second;

         SGT_DEBUG(debug() << "\tNext scheduled time = " << nextSchedTime << " for simObject "
                           << schedComp->name() << std::endl);
         SGT_DEBUG(debug() << "\t\t(Start, current, end time = " << startTime_ << " " << currentTime_
                           << " " << endTime_ << ")." << std::endl);
      }

      if (nextSchedTime > currentTime_ && contingentUpdates_.size() > 0 && currentTime_ < endTime_)
      {
         // There are contingent updates pending.
         SimObject* contComp = *contingentUpdates_.begin();
         SGT_DEBUG(debug() << "\tContingent update simObject " << contComp->name() << " from "
                           << schedComp->time() << " to " << currentTime_ << std::endl);
         contingentUpdates_.erase(contingentUpdates_.begin()); // Remove from the set.
         // Before updating the simObject, we need to take it out of the scheduled updates set, because its
         // sort key might change.
         for (auto it = scheduledUpdates_.begin(); it != scheduledUpdates_.end(); ++it)
         {
            if (it->first == contComp)
            {
               scheduledUpdates_.erase(it);
               break;
            }
         }
         contComp->update(currentTime_); // Now do the update
         scheduledUpdates_.insert(std::make_pair(contComp, contComp->validUntil()));
            // ... and reinsert it in the scheduled updates set.
         result = true;
      }
      else if (scheduledUpdates_.size() > 0 && nextSchedTime < endTime_)
      {
         // There is a scheduled update to do next.
         if (nextSchedTime > currentTime_)
         {
            timestepWillStart_.trigger();
         }
         currentTime_ = nextSchedTime;
         SGT_DEBUG(debug() << "\tScheduled update simObject " << schedComp->name() << " from "
                           << schedComp->time() << " to " << currentTime_ << std::endl);

         // Remove the scheduled and possible contingent update. Note that if there is a contingent update, it was
         // inserted by an element that has already updated, and so it is safe to do the scheduled update in place of
         // the contingent update.
         scheduledUpdates_.erase(schedUpdateIt);
         contingentUpdates_.erase(schedComp);

         // Now perform the update.
         schedComp->update(currentTime_);
         scheduledUpdates_.insert(std::make_pair(schedComp, schedComp->validUntil())); // and reinsert it.
         result = true;
      }
      else
      {
         SGT_DEBUG(debug() << "\tNo update." << std::endl);
      }
      if (  contingentUpdates_.size() == 0 &&
            (scheduledUpdates_.size() == 0 || (scheduledUpdates_.begin()->second > currentTime_)))
      {
         // We've reached the end of this step.
         SGT_DEBUG(debug() << "\tTimestep completed at " << currentTime_ << std::endl);
         for (SimObject* comp : mod_->simObjects())
         {
            if (comp->time() == currentTime_)
            {
               SGT_DEBUG(debug() << "\tSimObject " << comp->name() << " completed timestep." << std::endl);
               comp->didCompleteTimestep().trigger();
            }
         }
         timestepDidComplete_.trigger();
      }

      return result;
   }

   bool Simulation::doTimestep()
   {
      Time time1 = currentTime_;
      // Do at least one step. This may push us over into the next timestep, in which case we should stop, unless
      // this was the very first timestep.
      bool result = doNextUpdate();
      Time timeToComplete = (time1 == posix_time::neg_infin) ? currentTime_ : time1;

      // Now finish off all contingent and scheduled updates in this step.
      while (result &&
             ((contingentUpdates_.size() > 0) ||
              (scheduledUpdates_.size() > 0 && (scheduledUpdates_.begin()->second == timeToComplete))))
      {
         result = result && doNextUpdate();
      }

      // Now bring up all lagging simObjects to the new time, if they have an update.
      Time time2 = currentTime_;
      while (result &&
             ((contingentUpdates_.size() > 0) ||
              (scheduledUpdates_.size() > 0 && (scheduledUpdates_.begin()->second == time2))))
      {
         result = result && doNextUpdate();
      }

      return result;
   }
}
