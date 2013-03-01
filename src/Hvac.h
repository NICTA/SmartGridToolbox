#ifndef HVAC_DOT_H
#define HVAC_DOT_H

#include <string>
#include "MGSim.h"

namespace MGSim
{
   /// Base class for simulation objects.
   class HVAC
   {
      public:
         /// Constructor.
         explicit HVAC(const std::string & name) : Object(name)
         {
            // Empty.
         }

         /// Pure virtual destructor.
         virtual ~HVAC() = 0;

         const std::string & GetName() const
         {
            return name_;
         }

         /// Bring state up to time toTime.
         /** @param toTime */
         virtual void UpdateState(TimestampType toTimestep);
         
         TimestampType GetTimestep();

      private:
         float tsp;
   };
}

#endif // HVAC_DOT_H
