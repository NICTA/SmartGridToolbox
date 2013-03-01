#ifndef OBJECT_DOT_H
#define OBJECT_DOT_H

#include <string>
#include "MGSim.h"

namespace MGSim
{
   /// Base class for simulation objects.
   class Object
   {
      public:
         /// Constructor.
         explicit Object(const std::string & name) : name_(name)
         {
            // Empty.
         }

         /// Pure virtual destructor.
         virtual ~Object() = 0;

         const std::string & GetName() const
         {
            return name_;
         }

         /// Bring state up to time toTime.
         /** @param toTime */
         virtual void UpdateState(TimestampType toTimestep);
         
         TimestampType GetTimestep();

      private:
         Object(const Object & from); // Forbid copies. No implementation.

      private:
         std::string name_;

   };
}

#endif // OBJECT_DOT_H
