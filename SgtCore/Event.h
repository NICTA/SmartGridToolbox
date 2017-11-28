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

#ifndef EVENT_DOT_H
#define EVENT_DOT_H

#include <SgtCore/Common.h>

#include <algorithm>
#include <functional>
#include <set>
#include <string>

namespace Sgt
{
    class Event;
    class OwnedAction;

    /// @brief An action that gets performed when an event is triggered.
    ///
    /// Actions are stored as a set in the event in question. Anyone may register an action. Deregistration is
    /// not currently supported.
    /// @ingroup Foundation
    class Action
    {
        friend class Event;

        public:

        Action() = default;

        Action(const std::function<void ()>& function, const std::string& description) :
            function_(function), 
            description_(description)
        {
            // Empty.
        }

        Action(const Action&) = delete;
        Action(Action&&) = delete;
        void operator=(const Action&) = delete;
        void operator=(Action&&) = delete;

        ~Action();

        void reset(const std::function<void ()>& function, const std::string& description = "");

        void addTrigger(const Event& event);

        void removeTrigger(const Event& event);

        void clearTriggers();

        const std::string& description() const
        {
            return description_;
        }

        void perform() const {function_();}

        private:
        std::set<const Event*> triggers_;
        std::function<void ()> function_;
        std::string description_;
    };

    /// @brief An event, when triggered, performs all of its registered actions.
    ///
    /// Actions are stored as a set in the event in question. Anyone may register an action. Deregistration is
    /// not currently supported.
    /// @ingroup Foundation
    class Event
    {
        friend class Action;

        public:
        Event(const std::string& description)
        {
            setDescription(description);
        }

        ~Event();

        void trigger() const;

        const std::string& description() const
        {
            return description_;
        }

        void setDescription(const std::string& description)
        {
            description_ = description;
            triggerThis_.description_ = "Trigger " + description_ + ".";
        }

        bool isEnabled() const
        {
            return isEnabled_;
        }

        void setIsEnabled(bool isEnabled)
        {
            isEnabled_ = isEnabled;
        }

        void addTrigger(const Event& trigger)
        {
            triggerThis_.addTrigger(trigger);
        }

        void removeTrigger(const Event& trigger)
        {
            triggerThis_.removeTrigger(trigger);
        }

        void clearTriggers()
        {
            triggerThis_.clearTriggers();
        }

        private:
        mutable std::set<Action*> actions_;
        std::string description_;
        bool isEnabled_{true};
        Action triggerThis_{[this](){trigger();}, ""};
    };
}

#endif // EVENT_DOT_H
