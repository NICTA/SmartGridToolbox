// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Simulation.h"
#include "WeakOrder.h"

#include <algorithm>

namespace Sgt
{
    void Simulation::initialize()
    {
        sgtLogMessage() << "Simulation initialize(): " << std::endl;
        LogIndent indent;
        if (debugLogLevel() >= LogLevel::VERBOSE)
        {
            sgtLogDebug(LogLevel::VERBOSE) << "Components before initialize:" << std::endl;
            {
                LogIndent indent;
                logComponents();
            }
        }

        for (std::size_t i = 0; i < simComps_.size(); ++i)
        {
            simComps_[i]->setRank(static_cast<int>(i));
        }

        WoGraph g(simComps_.size());
        for (std::size_t i = 0; i < simComps_.size(); ++i)
        {
            for (auto dep : simComps_[i]->dependencies())
            {
                // i depends on dep->rank().
                g.link(static_cast<std::size_t>(dep->rank()), i);
            }
        }
        g.weakOrder();
       
        for (std::size_t i = 0; i < simComps_.size(); ++i)
        {
            std::size_t j = g.nodes()[i]->index();
            simComps_[j]->setRank(static_cast<int>(i));
        }

        std::sort(simComps_.begin(), simComps_.end(), 
                [](const ComponentPtr<SimComponent>& lhs, const ComponentPtr<SimComponent>& rhs)
                {return lhs->rank() < rhs->rank();});

        scheduledUpdates_.clear();
        for (auto comp : simComps_)
        {
            comp->initialize();
            scheduledUpdates_.insert(std::make_pair(comp, startTime_));
            comp->insertContingentUpdateAction_.reset([this, comp](){contingentUpdates_.insert(comp);},
                    "Simulation insert contingent update of " + comp->componentType() + " " + comp->id());
            comp->insertContingentUpdateAction_.addTrigger(comp->needsUpdate());
        }
        currentTime_ = TimeSpecialValues::neg_infin;
        // Contingent updates may have been inserted during initialization process e.g. when setting up setpoints etc.
        contingentUpdates_.clear();

        doTimestep(); // Do the first timestep, to advance the start time for -infinity to startTime_.

        isValid_ = true;
        
        if (debugLogLevel() >= LogLevel::VERBOSE)
        {
            sgtLogDebug(LogLevel::VERBOSE) << "Components after initialize:" << std::endl;
            {
                LogIndent indent;
                logComponents();
            }
        }
        
        indent.out();
        sgtLogMessage() << "Simulation initialize() finished." << std::endl;
    }

    // TODO: can we tidy up the logic in this function?
    void Simulation::doNextUpdate()
    {
        sgtLogMessage(LogLevel::VERBOSE) << "Simulation doNextUpdate(): " << std::endl;
        LogIndent indent;
        sgtLogDebug(LogLevel::VERBOSE) << "Number of scheduled = " << scheduledUpdates_.size() << std::endl;
        sgtLogDebug(LogLevel::VERBOSE) << "Number of contingent = " << contingentUpdates_.size() << std::endl;

        Time nextSchedTime = TimeSpecialValues::pos_infin;
        ComponentPtr<SimComponent> schedComp;
        auto schedUpdateIt = scheduledUpdates_.begin();

        if (scheduledUpdates_.size() > 0)
        {
            schedComp = schedUpdateIt->first;
            nextSchedTime = schedUpdateIt->second;

            sgtLogDebug(LogLevel::VERBOSE) << "Next scheduled time = "
                << localTimeString(nextSchedTime) << " for SimComponent " << schedComp->id() << std::endl;
            sgtLogDebug(LogLevel::VERBOSE) << "(Start, current, end time = "
                << localTimeString(startTime_) << " " << localTimeString(currentTime_)
                << " " << localTimeString(endTime_) << ")." << std::endl;
        }

        bool simIsFinished = false;

        if (nextSchedTime > currentTime_ && contingentUpdates_.size() > 0 && currentTime_ <= endTime_)
        {
            // There are contingent updates pending.
            auto contComp = *contingentUpdates_.begin();
            sgtLogMessage(LogLevel::VERBOSE) << "Contingent update SimComponent " << contComp->id() << " from "
                << localTimeString(contComp->lastUpdated())
                << " to " << localTimeString(currentTime_) << std::endl;
            LogIndent indent;
            contingentUpdates_.erase(contingentUpdates_.begin()); // Remove from the set.
            // Before updating the SimComponent, we need to take it out of the scheduled updates set, because its
            // sort key might change.
            for (auto it = scheduledUpdates_.begin(); it != scheduledUpdates_.end(); ++it)
            {
                if (static_cast<const SimComponent*>(it->first) == static_cast<const SimComponent*>(contComp))
                {
                    scheduledUpdates_.erase(it);
                    break;
                }
            }
            indent.in();
            // Now perform the update...
            contComp->update(currentTime_);
            indent.out();

            // ... and try to reinsert component in scheduled updates.
            tryInsertScheduledUpdate(contComp);
        }
        else if (scheduledUpdates_.size() > 0 && nextSchedTime <= endTime_)
        {
            // There is a scheduled update to do next.
            bool isNew = (nextSchedTime > currentTime_);

            currentTime_ = nextSchedTime;

            if (isNew)
            {
                sgtLogMessage() << "Simulation new timestep at time " << localTimeString(currentTime_) << std::endl;
                timestepWillStart_.trigger();
            }

            sgtLogMessage(LogLevel::VERBOSE) << "Scheduled update SimComponent " << schedComp->id()
                << " from " << localTimeString(schedComp->lastUpdated())
                << " to " << localTimeString(currentTime_) << std::endl;

            // Remove the scheduled and possible contingent update. Note that if there is a contingent update, it was
            // inserted by an element that has already updated, and so it is safe to do the scheduled update in place
            // of the contingent update.
            scheduledUpdates_.erase(schedUpdateIt);
            contingentUpdates_.erase(schedComp);

            // Now perform the update...
            indent.in();
            schedComp->update(currentTime_);
            indent.out();

            // ... and try to reinsert it.
            tryInsertScheduledUpdate(schedComp);
        }
        else
        {
            simIsFinished = true;
        }

        if (contingentUpdates_.size() == 0 &&
                (scheduledUpdates_.size() == 0 || (scheduledUpdates_.begin()->second > currentTime_)))
        {
            // We've reached the end of this step.
            for (auto comp : simComps_)
            {
                if (comp->lastUpdated() == currentTime_)
                {
                    sgtLogDebug(LogLevel::VERBOSE) << "Calling finalizeState for " << comp->id() << std::endl;
                    comp->finalizeTimestep();
                    sgtLogDebug(LogLevel::VERBOSE) << "SimComponent " << comp->id() << " completed timestep."
                        << std::endl;
                    comp->didCompleteTimestep().trigger();
                }
            }
            sgtLogMessage() << "Timestep completed at " << localTimeString(currentTime_) << std::endl;
            timestepDidComplete_.trigger();
        }
        sgtLogDebug(LogLevel::VERBOSE) << "After Simulation doNextUpdate():" << std::endl;
        indent.in();
        sgtLogDebug(LogLevel::VERBOSE) << "Number of scheduled = " << scheduledUpdates_.size() << std::endl;
        sgtLogDebug(LogLevel::VERBOSE) << "Number of contingent = " << contingentUpdates_.size() << std::endl;
        indent.out();

        indent.out();
        sgtLogMessage(LogLevel::VERBOSE) << "Simulation doNextUpdate() finished." << std::endl;
        if (simIsFinished)
        {
            sgtLogMessage() << "Simulation is finished." << std::endl;
            for (auto& comp: simComps_)
            {
                sgtLogDebug(LogLevel::VERBOSE) << "Calling finalizeSimulation for " << comp->id() << std::endl;
                comp->finalizeSimulation();
            }
        }
    }

    void Simulation::doTimestep()
    {
        Time time1 = currentTime_;
        LogIndent indent;
        // Do at least one step. This may push us over into the next timestep, in which case we should stop, unless
        // this was the very first timestep.
        doNextUpdate();
        Time timeToComplete = (time1 == TimeSpecialValues::neg_infin) ? currentTime_ : time1;

        // Now finish off all contingent and scheduled updates in this step.
        while ((contingentUpdates_.size() > 0) ||
                (scheduledUpdates_.size() > 0 && (scheduledUpdates_.begin()->second == timeToComplete)))
        {
            doNextUpdate();
        }

        // Now bring up all lagging SimComponents to the new time, if they have an update.
        Time time2 = currentTime_;
        while ((contingentUpdates_.size() > 0) ||
                (scheduledUpdates_.size() > 0 && (scheduledUpdates_.begin()->second == time2)))
        {
            doNextUpdate();
        }
    }

    void Simulation::logComponents() const
    {
        for (auto comp : simComps_)
        {
            sgtLogDebug(LogLevel::VERBOSE) << comp->id() << " " << comp->rank() << std::endl;
            LogIndent indent;
            for (auto dep : comp->dependencies())
            {
                sgtLogDebug(LogLevel::VERBOSE) << dep->id() << " " << dep->rank() << std::endl;
            }
        }
    }

    void Simulation::tryInsertScheduledUpdate(const ComponentPtr<SimComponent>& schedComp)
    {
        sgtLogDebug(LogLevel::VERBOSE) << "TryInsertScheduledUpdate: " << schedComp->id() << std::endl;
        LogIndent indent;
        Time nextUpdate = schedComp->validUntil();
        sgtLogDebug(LogLevel::VERBOSE) << "nextUpdate = " << localTimeString(nextUpdate) << std::endl;
        sgtLogDebug(LogLevel::VERBOSE) << "endTime_ = " << localTimeString(endTime_) << std::endl;
        if ((!nextUpdate.is_special()) && nextUpdate <= endTime_)
        {
            sgtLogDebug(LogLevel::VERBOSE) << "Inserting " << schedComp->id()
                << ": nextUpdate = " << localTimeString(nextUpdate) << std::endl;
            scheduledUpdates_.insert(std::make_pair(schedComp, nextUpdate));
        }
    };
}
