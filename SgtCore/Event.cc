// Copyright 2015-2016 National ICT Australia Limited (NICTA)
// Copyright 2016-2019 The Australian National University
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

#include "Event.h"

#include <SgtCore/Common.h>

namespace Sgt
{
    Action::~Action()
    {
        while (!triggers_.empty())
        {
            removeTrigger(**triggers_.begin());
        }
    }

    void Action::reset(const std::function<void ()>& function, const std::string& description)
    {
        function_ = function;
        if (description != "") description_ = description;
        clearTriggers();
    }
            
    void Action::addTrigger(const Event& event)
    {
        triggers_.insert(&event);
        event.actions_.insert(this); 
    }
    
    void Action::removeTrigger(const Event& event)
    {
        triggers_.erase(&event);
        event.actions_.erase(this); 
    }
    
    void Action::clearTriggers()
    {
        while (!triggers_.empty())
        {
            removeTrigger(**triggers_.begin());
        }
    }
            
    Event::~Event()
    {
        while (!actions_.empty())
        {
            (**actions_.begin()).removeTrigger(*this);
        }
    }

    void Event::trigger() const
    {
        if (isEnabled_)
        {
            sgtLogDebug(LogLevel::VERBOSE) << "Event was triggered: " << description_ << std::endl;
            sgtLogIndent();
            for (const Action* action : actions_)
            {
                sgtLogDebug(LogLevel::VERBOSE) << "Event perform action: " << action->description() << std::endl;
                {
                    sgtLogIndent();
                    action->perform();
                }
            }
        }
        else
        {
            sgtLogDebug(LogLevel::VERBOSE) << "Event not triggered because it is not enabled: " << description_ 
                << std::endl;
        }
    }
}
