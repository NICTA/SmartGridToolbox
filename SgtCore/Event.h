#ifndef EVENT_DOT_H
#define EVENT_DOT_H

#include <SgtCore/Common.h>

#include <functional>
#include <list>
#include <string>

namespace SmartGridToolbox
{
   /// @brief An action that gets performed when an event is triggered.
   ///
   /// Actions are stored as a list in the event in question. Anyone may register an action. Deregistration is
   /// not currently supported.
   /// @ingroup Core
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
   /// @ingroup Core
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
            SGT_DEBUG(Log().debug() << "Event: " << description_ << ": addAction: " << description << std::endl);
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

         bool isActive() const
         {
            return isActive_;
         }

         void setIsActive(bool isActive)
         {
            isActive_ = isActive;
         }

      private:
         std::list<Action> actions_;
         std::string description_;
         bool isActive_{true};
   };
}

#endif // EVENT_DOT_H
