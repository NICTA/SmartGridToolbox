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
    Simulation::ConstSimCompVec Simulation::simComponents() const
    {
        ConstSimCompVec result(simCompVec_.size());
        std::copy(simCompVec_.begin(), simCompVec_.end(), result.begin());
        return result;
    }

    void Simulation::addOrReplaceGenericSimComponent(std::shared_ptr<SimComponent> simComp, bool allowReplace)
    {
        SimCompMap::iterator it1 = simCompMap_.find(simComp->id());
        if (it1 != simCompMap_.end())
        {
            if (allowReplace)
            {
                it1->second = simComp;
            }
            else
            {
                sgtError("SimComponent " << simComp->id() << " occurs more than once in the model.");
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
        sgtLogMessage(LogLevel::VERBOSE) << "Simulation initialize(): " << std::endl;
        LogIndent indent;
        if (debugLogLevel() >= LogLevel::VERBOSE)
        {
            sgtLogDebug(LogLevel::VERBOSE) << "Components before initialize:" << std::endl;
            {
                LogIndent indent;
                logComponents();
            }
        }

        for (std::size_t i = 0; i < simCompVec_.size(); ++i)
        {
            simCompVec_[i]->setRank(static_cast<int>(i));
        }

        WoGraph g(simCompVec_.size());
        for (std::size_t i = 0; i < simCompVec_.size(); ++i)
        {
            for (auto dep : simCompVec_[i]->dependencies())
            {
                // i depends on dep->rank().
                g.link(static_cast<std::size_t>(dep.lock()->rank()), i);
            }
        }
        g.weakOrder();

        // g.nodes() now specifies a permutation to be applied to simCompVec_.
        SimCompVec perm;
        perm.reserve(simCompVec_.size());
        for (auto& nd : g.nodes())
        {
            perm.push_back(simCompVec_[nd->index()]);
        }
        simCompVec_ = perm;

        // Reset the evaluation rank.
        for (std::size_t i = 0; i < simCompVec_.size(); ++i)
        {
            simCompVec_[i]->setRank(static_cast<int>(i));
        }

        scheduledUpdates_.clear();
        for (auto comp : simCompVec_)
        {
            comp->initialize();
            scheduledUpdates_.insert(std::make_pair(comp, startTime_));
            comp->needsUpdate().addAction([this, comp]() {contingentUpdates_.insert(comp);},
            std::string("Simulation insert contingent update of ") + comp->componentType() + " " + comp->id());
        }
        currentTime_ = posix_time::neg_infin;
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
    }

    // TODO: can we tidy up the logic in this function?
    void Simulation::doNextUpdate()
    {
        sgtLogMessage(LogLevel::VERBOSE) << "Simulation doNextUpdate(): " << std::endl;
        LogIndent indent;
        sgtLogDebug(LogLevel::VERBOSE) << "Number of scheduled = " << scheduledUpdates_.size() << std::endl;
        sgtLogDebug(LogLevel::VERBOSE) << "Number of contingent = " << contingentUpdates_.size() << std::endl;

        Time nextSchedTime = posix_time::pos_infin;
        SimCompPtr schedComp(nullptr);
        auto schedUpdateIt = scheduledUpdates_.begin();

        if (scheduledUpdates_.size() > 0)
        {
            schedComp = schedUpdateIt->first;
            nextSchedTime = schedUpdateIt->second;

            sgtLogDebug(LogLevel::VERBOSE) << "Next scheduled time = " << nextSchedTime << " for simComponent "
                << schedComp->id() << std::endl;
            sgtLogDebug(LogLevel::VERBOSE) << "(Start, current, end time = " << startTime_ << " " << currentTime_
                << " " << endTime_ << ")." << std::endl;
        }

        if (nextSchedTime > currentTime_ && contingentUpdates_.size() > 0 && currentTime_ <= endTime_)
        {
            // There are contingent updates pending.
            auto contComp = *contingentUpdates_.begin();
            sgtLogMessage(LogLevel::VERBOSE) << "Contingent update simComponent " << contComp->id() << " from "
                << contComp->lastUpdated() << " to " << currentTime_ << std::endl;
            LogIndent indent;
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
            sgtLogDebug(LogLevel::VERBOSE) << "About to perform update..." << std::endl;
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
            if (nextSchedTime > currentTime_)
            {
                sgtLogMessage(LogLevel::VERBOSE) << "Timestep " << currentTime_ << " -> " << nextSchedTime
                    << " will start" << std::endl;
                timestepWillStart_.trigger();
            }
            currentTime_ = nextSchedTime;
            sgtLogMessage(LogLevel::VERBOSE) << "Scheduled update simComponent " << schedComp->id() << " from "
                << schedComp->lastUpdated() << " to " << currentTime_ << std::endl;

            // Remove the scheduled and possible contingent update. Note that if there is a contingent update, it was
            // inserted by an element that has already updated, and so it is safe to do the scheduled update in place
            // of the contingent update.
            scheduledUpdates_.erase(schedUpdateIt);
            contingentUpdates_.erase(schedComp);

            // Now perform the update...
            sgtLogDebug(LogLevel::VERBOSE) << "About to perform update..." << std::endl;
            indent.in();
            schedComp->update(currentTime_);
            indent.out();

            // ... and try to reinsert it.
            tryInsertScheduledUpdate(schedComp);
        }
        else
        {
            sgtLogDebug() << "No update." << std::endl;
        }
        if (contingentUpdates_.size() == 0 &&
                (scheduledUpdates_.size() == 0 || (scheduledUpdates_.begin()->second > currentTime_)))
        {
            // We've reached the end of this step.
            sgtLogMessage(LogLevel::VERBOSE) << "Timestep completed at " << currentTime_ << std::endl;
            for (auto comp : simCompVec_)
            {
                if (comp->lastUpdated() == currentTime_)
                {
                    sgtLogDebug(LogLevel::VERBOSE) << "SimComponent " << comp->id() << " completed timestep."
                        << std::endl;
                    comp->didCompleteTimestep().trigger();
                }
            }
            timestepDidComplete_.trigger();
        }
        sgtLogDebug(LogLevel::VERBOSE) << "After Simulation doNextUpdate():" << std::endl;
        indent.in();
        sgtLogDebug(LogLevel::VERBOSE) << "Number of scheduled = " << scheduledUpdates_.size() << std::endl;
        sgtLogDebug(LogLevel::VERBOSE) << "Number of contingent = " << contingentUpdates_.size() << std::endl;
        indent.out();
    }

    void Simulation::doTimestep()
    {
        sgtLogMessage() << "Simulation doTimestep(): " << std::endl;
        LogIndent indent;
        Time time1 = currentTime_;
        // Do at least one step. This may push us over into the next timestep, in which case we should stop, unless
        // this was the very first timestep.
        doNextUpdate();
        Time timeToComplete = (time1 == posix_time::neg_infin) ? currentTime_ : time1;

        // Now finish off all contingent and scheduled updates in this step.
        while ((contingentUpdates_.size() > 0) ||
                (scheduledUpdates_.size() > 0 && (scheduledUpdates_.begin()->second == timeToComplete)))
        {
            doNextUpdate();
        }

        // Now bring up all lagging simComponents to the new time, if they have an update.
        Time time2 = currentTime_;
        while ((contingentUpdates_.size() > 0) ||
                (scheduledUpdates_.size() > 0 && (scheduledUpdates_.begin()->second == time2)))
        {
            doNextUpdate();
        }
    }

    void Simulation::logComponents()
    {
        for (auto comp : simCompVec_)
        {
            sgtLogMessage() << comp->id() << " " << comp->rank() << std::endl;
            LogIndent indent;
            for (auto dep : comp->dependencies())
            {
                sgtLogMessage() << dep.lock()->id() << " " << dep.lock()->rank() << std::endl;
            }
        }
    }

    std::shared_ptr<const SimComponent> Simulation::genericSimComponent(const std::string& id,
            bool crashOnFail) const
    {
        std::shared_ptr<const SimComponent> result = nullptr;
        SimCompMap::const_iterator it = simCompMap_.find(id);
        if (it != simCompMap_.end())
        {
            result = it->second;
        }
        else if (crashOnFail)
        {
            sgtError("Component " << id << " was requested but was not found in the simulation.");
        }
        return result;
    }

    std::shared_ptr<const TimeSeriesBase> Simulation::genericTimeSeries(const std::string& id, bool crashOnFail) const
    {
        std::shared_ptr<const TimeSeriesBase> result = nullptr;
        TimeSeriesMap::const_iterator it = timeSeriesMap_.find(id);
        if (it != timeSeriesMap_.end())
        {
            result = it->second;
        }
        else if (crashOnFail)
        {
            sgtError("Time series " << id << " was requested but was not found in the simulation.");
        }
        return result;
    }

    void Simulation::tryInsertScheduledUpdate(SimCompPtr schedComp)
    {
        sgtLogDebug(LogLevel::VERBOSE) << "TryInsertScheduledUpdate: " << schedComp->id() << std::endl;
        LogIndent indent;
        Time nextUpdate = schedComp->validUntil();
        sgtLogDebug(LogLevel::VERBOSE) << "nextUpdate = " << nextUpdate << std::endl;
        sgtLogDebug(LogLevel::VERBOSE) << "endTime_ = " << endTime_ << std::endl;
        if (nextUpdate <= endTime_) 
        {
            sgtLogDebug(LogLevel::VERBOSE) << "Inserting " << schedComp->id() << ": nextUpdate = " << nextUpdate 
                << std::endl;
            scheduledUpdates_.insert(std::make_pair(schedComp, nextUpdate));
        }
    };
}
