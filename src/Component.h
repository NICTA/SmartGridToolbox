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
         explicit Component(const std::string & name) : name_(name)
         {
            // Empty.
         }

         /// Pure virtual destructor.
         virtual ~Component()
         {
            // Empty.
         }

         const std::string & GetName() const
         {
            return name_;
         }

         /// Bring state up to time toTime.
         /** @param toTime */
         virtual void AdvanceToTime(Timestamp t) = 0;
         
         /// Get the current step for the object.
         Timestamp GetTimestep()
         {
            return t_;
         }

      private:

      private:
         std::string name_;
         Timestamp t_;
   };
}

#endif // COMPONENT_DOT_H
