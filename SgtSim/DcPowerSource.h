#ifndef DC_POWER_SOURCE_DOT_H
#define DC_POWER_SOURCE_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Component.h>

namespace SmartGridToolbox
{
   /// @brief DC power source.
   ///
   /// Abstract base class for any object that can provide a source of DC power, i.e. a single real power.
   /// @ingroup PowerFlowCore
   class DcPowerSourceInterface : virtual public SimComponentInterface
   {
      public:
         
      /// @name DC Power.
      /// @{
         
         virtual double PDc() const = 0;

      /// @}
   };

   class GenericDcPowerSource : public SimComponent, virtual public DcPowerSourceInterface
   {
      /// @name Overridden member functions from SimComponent.
      /// @{
      
      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;
      
      /// @}
      
      /// @name Overridden member functions from DcPowerSourceAbc.
      /// @{
      
      public:
         virtual double PDc() const {return PDc_;}
      
      /// @}
      
      /// @name My public member functions.
      /// @{
      
      public:
         GenericDcPowerSource(const std::string& id) : SimComponent(id), PDc_(0.0) {}

         void setPDc(double PDc) {PDc_ = PDc; needsUpdate().trigger();}

      /// @}

      public:
         double PDc_;
   };
}

#endif // DC_POWER_SOURCE_DOT_H
