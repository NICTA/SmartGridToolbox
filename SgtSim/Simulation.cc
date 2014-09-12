#include "Simulation.h"
#include "WeakOrder.h"

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
   
   Simulation::ConstSimCompVec Simulation::simComponents() const
   {
      ConstSimCompVec result(simCompVec_.size());
      std::copy(simCompVec_.begin(), simCompVec_.end(), result.begin());
      return result;
   }

   void Simulation::addOrReplaceGenericSimComponent(std::shared_ptr<SimComponentAbc> simComp, bool allowReplace)
   {
      Log().message() << "Adding simComponent " << simComp->id() << " of type " 
         << simComp->componentType() << " to model." << std::endl;
      Indent _;

      SimCompMap::iterator it1 = simCompMap_.find(simComp->id());
      if (it1 != simCompMap_.end())
      {
         if (allowReplace)
         {
            it1->second = simComp;
            Log().message() << "SimComponent " << simComp->id() << " replaced in model." << std::endl;
         }
         else
         {
            Log().fatal() << "SimComponent " << simComp->id() << " occurs more than once in the model!" << std::endl;
         }
      }
      else
      {
         simCompVec_.push_back(simComp);
         simCompMap_[simComp->id()] = simComp;
      }
   }

   void Simulation::initialize()
   {
      scheduledUpdates_.clear();
      for (auto comp : simCompVec_)
      {
         comp->initialize();
         scheduledUpdates_.insert(std::make_pair(comp, startTime_));
         comp->needsUpdate().addAction([this, comp](){contingentUpdates_.insert(comp);},
               std::string("Simulation insert contingent update of ") + comp->componentType() + " " + comp->id());
      }
      currentTime_ = posix_time::neg_infin;
      // Contingent updates may have been inserted during initialization process e.g. when setting up setpoints etc.
      contingentUpdates_.clear();
      doTimestep(); // Do the first timestep, to advance the start time for -infinity to startTime_.
   }

   // TODO: can we tidy up the logic in this function?
   bool Simulation::doNextUpdate()
   {
      SGT_DEBUG(Log().debug() << "Simulation doNextUpdate(): " << std::endl);
      SGT_DEBUG(Log().debug() << "Number of scheduled = " << scheduledUpdates_.size() << std::endl);
      SGT_DEBUG(Log().debug() << "Number of contingent = " << contingentUpdates_.size() << std::endl);
      SGT_DEBUG(Indent _);

      bool result = false;

      Time nextSchedTime = posix_time::pos_infin;
      SimCompPtr schedComp(nullptr);
      auto schedUpdateIt = scheduledUpdates_.begin();

      if (scheduledUpdates_.size() > 0)
      {
         schedComp = schedUpdateIt->first;
         nextSchedTime = schedUpdateIt->second;

         SGT_DEBUG(Log().debug() << "Next scheduled time = " << nextSchedTime << " for simComponent "
                           << schedComp->id() << std::endl);
         SGT_DEBUG(Log().debug() << "(Start, current, end time = " << startTime_ << " " << currentTime_
                           << " " << endTime_ << ")." << std::endl);
      }

      if (nextSchedTime > currentTime_ && contingentUpdates_.size() > 0 && currentTime_ < endTime_)
      {
         // There are contingent updates pending.
         auto contComp = *contingentUpdates_.begin();
         SGT_DEBUG(Log().debug() << "Contingent update simComponent " << contComp->id() << " from "
                           << schedComp->time() << " to " << currentTime_ << std::endl);
         SGT_DEBUG(Indent _);
         contingentUpdates_.erase(contingentUpdates_.begin()); // Remove from the set.
         // Before updating the simComponent, we need to take it out of the scheduled updates set, because its
         // sort key might change.
         for (auto it = scheduledUpdates_.begin(); it != scheduledUpdates_.end(); ++it)
         {
            if (it->first == contComp)
            {
               scheduledUpdates_.erase(it);
               break;
            }
         }
         SGT_DEBUG(Log().debug() << "About to perform update..." << std::endl);
         SGT_DEBUG({Indent _;)
         contComp->update(currentTime_); // Now do the update
         SGT_DEBUG(})
         SGT_DEBUG(Log().debug() << "... Done" << std::endl);
         scheduledUpdates_.insert(std::make_pair(contComp, contComp->validUntil()));
            // ... and reinsert it in the scheduled updates set.
         result = true;
      }
      else if (scheduledUpdates_.size() > 0 && nextSchedTime < endTime_)
      {
         // There is a scheduled update to do next.
         if (nextSchedTime > currentTime_)
         {
            SGT_DEBUG(Log().debug() << "Timestep " << currentTime_ << " -> " << nextSchedTime
                  << " will start" << std::endl);
            timestepWillStart_.trigger();
         }
         currentTime_ = nextSchedTime;
         SGT_DEBUG(Log().debug() << "Scheduled update simComponent " << schedComp->id() << " from "
                           << schedComp->time() << " to " << currentTime_ << std::endl);

         // Remove the scheduled and possible contingent update. Note that if there is a contingent update, it was
         // inserted by an element that has already updated, and so it is safe to do the scheduled update in place of
         // the contingent update.
         scheduledUpdates_.erase(schedUpdateIt);
         contingentUpdates_.erase(schedComp);

         // Now perform the update.
         SGT_DEBUG(Log().debug() << "About to perform update..." << std::endl);
         SGT_DEBUG({Indent _;)
         schedComp->update(currentTime_);
         SGT_DEBUG(})
         SGT_DEBUG(Log().debug() << "... Done" << std::endl);
         scheduledUpdates_.insert(std::make_pair(schedComp, schedComp->validUntil())); // and reinsert it.
         result = true;
      }
      else
      {
         SGT_DEBUG(Log().debug() << "No update." << std::endl);
      }
      if (  contingentUpdates_.size() == 0 &&
            (scheduledUpdates_.size() == 0 || (scheduledUpdates_.begin()->second > currentTime_)))
      {
         // We've reached the end of this step.
         SGT_DEBUG(Log().debug() << "Timestep completed at " << currentTime_ << std::endl);
         for (auto comp : simCompVec_)
         {
            if (comp->time() == currentTime_)
            {
               SGT_DEBUG(Log().debug() << "SimComponent " << comp->id() << " completed timestep." << std::endl);
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

      // Now bring up all lagging simComponents to the new time, if they have an update.
      Time time2 = currentTime_;
      while (result &&
             ((contingentUpdates_.size() > 0) ||
              (scheduledUpdates_.size() > 0 && (scheduledUpdates_.begin()->second == time2))))
      {
         result = result && doNextUpdate();
      }

      return result;
   }

   void Simulation::validate()
   {
      SGT_DEBUG
      (
         Log().debug() << "Model before validation:" << std::endl;
         for (auto comp : simCompVec_)
         {
            Log().debug() << "\t" << comp->name() << " " << comp->rank() << std::endl;
            for (auto dep : comp->dependencies())
            {
               Log().debug() << "\t\t" << dep->name() << " " << dep->rank() << std::endl;
            }
         }
      )

      for (int i = 0; i < simCompVec_.size(); ++i)
      {
         simCompVec_[i]->setRank(i);
      }

      WoGraph g(simCompVec_.size());
      for (int i = 0; i < simCompVec_.size(); ++i)
      {
         // At this point, rank of component i is i.
         for (auto dep : simCompVec_[i]->dependencies())
         {
            // i depends on dep->rank().
            g.link(dep.lock()->rank(), i);
         }
      }
      g.weakOrder();
      int newRank = 0;
      for (int i = 0; i < g.size(); ++i)
      {
         if (i > 0 && g.nodes()[i-1]->dominates(*g.nodes()[i]))
         {
            ++newRank;
         }
         int idx_i = g.nodes()[i]->index();
         simCompVec_[idx_i]->setRank(newRank);
      }
      std::sort(simCompVec_.begin(), simCompVec_.end(), [](SimCompConstPtr lhs, SimCompConstPtr rhs) -> bool 
            {
               return ((lhs->rank() < rhs->rank()) || ((lhs->rank() == rhs->rank()) && (lhs->id() < rhs->id())));
            });
      isValid_ = true;
      Log().message() << "Model after validation:" << std::endl;
      for (auto comp : simCompVec_)
      {
         Log().message() << "\t" << comp->id() << " " << comp->rank() << std::endl;
         for (auto dep : comp->dependencies())
         {
            Log().message() << "\t\t" << dep.lock()->id() << " " << dep.lock()->rank() << std::endl;
         }
      }
   }
}
