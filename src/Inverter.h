#ifndef INVERTER_DOT_H
#define INVERTER_DOT_H

#include "ZipToGroundBase.h"

namespace SmartGridToolbox
{
   /// Inverter: DC power to n-phase AC converter.
   class Inverter : public ZipToGroundBase
   {
      /// @name Public overridden functions: from Component.
      /// @{
      public:
         virtual ptime getValidUntil() const override 
         {
            return pos_infin;
         }
      /// @}

      /// @name Private overridden functions: from Component.
      /// @{
      private:
         virtual void initializeState(ptime t) override {};
         virtual void updateState(ptime t0, ptime t1) override {};
      /// @}

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

#endif // INVERTER_DOT_H
