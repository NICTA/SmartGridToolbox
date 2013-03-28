#ifndef BUILDING_DOT_H
#define BUILDING_DOT_H

#include <string>
#include "Common.h"
#include "Component.h"

namespace SmartGridToolbox
{
   class HeatFlow;

   /// Base class for simulation objects.
   class Building : public Component
   {
      public:
         /// Constructor.
         explicit Building(const std::string & name) :
            Component(name)
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
         double tsp_;
   };
}

#endif // BUILDING_DOT_H
