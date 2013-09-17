#ifndef EVENT_DOT_H
#define EVENT_DOT_H

#include <functional>
#include <list>

namespace SmartGridToolbox
{
   class Component;

   class Action
   {
      public:
         Action(const std::function<void ()> & function, const std::string & description) : 
            function_(function), description_(description)
         {
            // Empty.
         }

         const std::string & description() const {return description_;}

         void perform() const {function_();}

      private:
         std::function<void ()> function_;
         std::string description_;
   };

   class Event
   {
      public:
         Event(const std::string & description) : description_(description)
         {
            // Empty. 
         }

         void addAction(const std::function<void ()> & action, const std::string & description)
         {
            SGT_DEBUG(debug() << "Event: " << description_ << ": addAction: " << description << std::endl);
            actions_.emplace_back(action, description);
         }

         void trigger();

         void clear()
         {
            actions_.clear();
         }

         const std::string & description() const
         {
            return description_;
         }

      private:
         std::list<Action> actions_;
         std::string description_;
   };
}

#endif // EVENT_DOT_H
