#ifndef DC_POWER_SOURCE_BASE_DOT_H
#define DC_POWER_SOURCE_BASE_DOT_H

#include <SmartGridToolbox/Component.h>

namespace SmartGridToolbox
{
   /// @brief DC power source.
   ///
   /// Abstract base class for any object that can provide a source of DC power, i.e. a single real power.
   /// @ingroup PowerFlowCore
   class DcPowerSourceBase : public Component
   {
      /// @name Overridden member functions from Component.
      /// @{
      
      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;

      /// @}
      
      public:
      /// @name My public member functions.
      /// @{
         
         DcPowerSourceBase(const std::string& name) : Component(name) {}
         virtual double PDc() const = 0;

      /// @}
   };
}

#endif // DC_POWER_SOURCE_BASE_DOT_H
