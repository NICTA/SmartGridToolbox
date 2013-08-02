#ifndef EVENT_DOT_H
#define EVENT_DOT_H

#include <functional>
#include <list>

namespace SmartGridToolbox
{
   class Component;

   class Event
   {
      public:
         typedef std::function<void()> Action;

      public:
         Event()
         {
            // Empty. 
         }

         void addAction(const Action & action)
         {
            actions_.push_back(action);
         }

         void trigger()
         {
            for (const Action & action : actions_) {action();}
         }

      private:
         std::list<Action> actions_;
   };
}

#endif // EVENT_DOT_H
