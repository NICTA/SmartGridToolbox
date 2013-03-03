#ifndef HVAC_DOT_H
#define HVAC_DOT_H

#include <string>
#include "MGSim.h"

namespace MGSim
{
   /// Base class for simulation objects.
   class Hvac : public HeatFlow
   {
      public:
         /// Constructor.
         explicit Hvac(const std::string & name) : Object(name)
         {
            // Empty.
         }

         /// Pure virtual destructor.
         virtual ~Hvac() = 0;

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
