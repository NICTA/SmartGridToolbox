#ifndef COMPONENT_DOT_H
#define COMPONENT_DOT_H

#include <string>
#include "Common.h"

namespace SmartGridToolbox
{
   /// Base class for simulation objects.
   class Component
   {
      public:
         /// Constructor.
         /** @param name the unique name */
         explicit Component(const std::string & name) : name_(name)
         {
            // Empty.
         }

         /// Pure virtual destructor.
         virtual ~Component()
         {
            // Empty.
         }

         /// Reset state of the object, time to timestamp t.
         /** @param t */
         virtual void Initialize(const ptime t)
         {
            t_ = t;
         }

         /// Get the name of the object.
         const std::string & GetName() const
         {
            return name_;
         }
         
         /// Get the current step for the object.
         ptime GetTimestamp()
         {
            return t_;
         }

         /// Bring state up to time t.
         /** @param t the initial timestamp */
         virtual void AdvanceToTime(ptime t) = 0;

      private:
         std::string name_;
         ptime t_;
   };
}

#endif // COMPONENT_DOT_H
