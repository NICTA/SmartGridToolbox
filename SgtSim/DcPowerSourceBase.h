#ifndef DC_POWER_SOURCE_BASE_DOT_H
#define DC_POWER_SOURCE_BASE_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Component.h>

namespace SmartGridToolbox
{
   /// @brief DC power source.
   ///
   /// Abstract base class for any object that can provide a source of DC power, i.e. a single real power.
   /// @ingroup PowerFlowCore
   class DcPowerSourceBase : public SimComponent, public Component
   {
      public:
      /// @{
         
         DcPowerSourceBase(const std::string& id) : Component(id) {}

      /// @}
         
      /// @name DC Power.
      /// @{
         
         virtual double PDc() const = 0;

      /// @}
      
      /// @name Overridden member functions from SimComponent.
      /// @{
      
      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;

      /// @}
   };
}

#endif // DC_POWER_SOURCE_BASE_DOT_H
