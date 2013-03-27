#ifndef BUILDING_DOT_H
#define BUILDING_DOT_H

#include <string>
#include "Common.h"
#include "Object.h"

namespace SmartGridToolbox
{
   class HeatFlow;

   /// Base class for simulation objects.
   class Building : public Object
   {
      public:
         /// Constructor.
         explicit Building(const std::string & name) :
            Object(name)
         {
            // Empty.
         }

         /// Virtual destructor.
         virtual ~Building()
         {
            // Empty.
         }

         void AttachHvac(Hvac & hvac)

         /// Bring state up to time toTime.
         /** @param toTime */
         virtual void UpdateState(TimestampType toTimestep);
         
         TimestampType GetTimestep();

      private:
         float tsp_;
   };
}

#endif // BUILDING_DOT_H
