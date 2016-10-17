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

#include "SimComponent.h"

namespace Sgt
{
    void SimComponent::initialize()
    {
        sgtLogDebug(LogLevel::VERBOSE) << "SimComponent " << id() << " initialize." << std::endl;
        willUpdate_.setDescription(id() + ": Will update");
        didUpdate_.setDescription(id() + ": Did update");
        needsUpdate_.setDescription(id() + ": Needs update");
        willStartNewTimestep_.setDescription(id() + ": Will start new timestep");
        didCompleteTimestep_.setDescription(id() + ": Did complete timestep");
        lastUpdated_ = posix_time::neg_infin;
        initializeState();
    }

    void SimComponent::update(Time t)
    {
        sgtLogDebug(LogLevel::VERBOSE) << "SimComponent " << id() << " update from " << lastUpdated_ << " to " 
            << t << std::endl;
        if (lastUpdated_ < t)
        {
            willStartNewTimestep_.trigger();
        }
        willUpdate_.trigger();
        updateState(t);
        lastUpdated_ = t;
        didUpdate_.trigger();
    }

    void SimComponent::dependsOn(const ConstComponentPtr<SimComponent>& comp, bool forceUpdate)
    {
        dependencies_.push_back(comp);
        if (forceUpdate)
        {
            needsUpdate().addTrigger(comp->didUpdate());
        }
    }
}
