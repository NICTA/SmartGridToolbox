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

#include <functional>
#include <list>
#include <string>

namespace Sgt
{
    /// @brief An action that gets performed when an event is triggered.
    ///
    /// Actions are stored as a list in the event in question. Anyone may register an action. Deregistration is
    /// not currently supported.
    /// @ingroup Foundation
    class Action
    {
        public:
            Action(const std::function<void ()>& function, const std::string& description)
                : function_(function), description_(description)
            {
                // Empty.
            }

            Action(const std::function<void ()>& function)
                : function_(function), description_("N/A")
            {
                // Empty.
            }

            const std::string& description() const
            {
                return description_;
            }

            void setDescription(const std::string& description)
            {
                description_ = description;
            }

            void perform() const {function_();}

        private:
            std::function<void ()> function_;
            std::string description_;
    };

    /// @brief An event, when triggered, performs all of its registered actions.
    ///
    /// Actions are stored as a list in the event in question. Anyone may register an action. Deregistration is
    /// not currently supported.
    /// @ingroup Foundation
    class Event
    {
        public:
            Event(const std::string& description) : description_(description)
            {
                // Empty.
            }

            Event() : description_("UNDEFINED")
            {
                // Empty.
            }

            void addAction(const std::function<void ()>& action, const std::string& description)
            {
                sgtLogDebug() << "Event: " << description_ << ": addAction: " << description << std::endl;
                actions_.emplace_back(action, description);
            }

            void trigger();

            void clear()
            {
                actions_.clear();
            }

            const std::string& description() const
            {
                return description_;
            }

            void setDescription(const std::string& description)
            {
                description_ = description;
            }

            bool isEnabled() const
            {
                return isEnabled_;
            }

            void setIsEnabled(bool isEnabled)
            {
                isEnabled_ = isEnabled;
            }

        private:
            std::list<Action> actions_;
            std::string description_;
            bool isEnabled_{true};
    };
}

#endif // EVENT_DOT_H
