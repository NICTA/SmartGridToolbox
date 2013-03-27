#ifndef COMPONENT_DOT_H
#define COMPONENT_DOT_H

#include <string>
#include "SmartGridToolbox.h"

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
         virtual ~Component() = 0;

         const std::string & GetName() const
         {
            return name_;
         }

         /// Bring state up to time toTime.
         /** @param toTime */
         virtual void UpdateState(TimestampType toTimestep);
         
         /// Get the current step for the object.
         TimestampType GetTimestep()
         {
            return ti_;
         }

      private:

      private:
         std::string name_;
         TimestampType ti_;
   };
}

#endif // COMPONENT_DOT_H
