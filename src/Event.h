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
         Event(const std::string & description) : description_(description)
         {
            // Empty. 
         }

         void addAction(const Action & action);

         void trigger();

         const std::string & getDescription() const
         {
            return description_;
         }

      private:
         std::list<Action> actions_;
         std::string description_;
   };
}

#endif // EVENT_DOT_H
