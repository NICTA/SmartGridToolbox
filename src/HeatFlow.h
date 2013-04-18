#ifndef HEAT_FLOW_DOT_H
#define HEAT_FLOW_DOT_H

#include <string>
#include "Common.h"
#include "Object.h"

namespace SmartGridToolbox
{
   class HeatFlow;

   /// ABC representing a heat source or sink. 
   class HeatFlow : public Object
   {
      public:
         /// Constructor.
         explicit HeatFlow(const std::string & name) :
            Object(name)
         {
            // Empty.
         }

         /// Virtual destructor.
         virtual ~HeatFlow()
         {
            // Empty.
         }

         virtual FloatType QDot() = 0;
   };
}

#endif // HEAT_FLOW_DOT_H
