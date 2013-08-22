#ifndef SIMPLE_INVERTER_DOT_H
#define SIMPLE_INVERTER_DOT_H

#include "ZipToGroundBase.h"

namespace SmartGridToolbox
{
   /// SimpleInverter: DC power to n-phase AC converter.
   class SimpleInverter : public InverterBase
   {
      /// @name My public member functions.
      /// @{
      public:
         virtual double getEfficiency(const double powerDC) = 0;
      /// @}

      /// @name Private member variables.
      /// @{
      public:
         double dcPower_;     ///< Power injection to connected bus (on AC side).
      /// @}
   };
}

#endif // SIMPLE_INVERTER_DOT_H
