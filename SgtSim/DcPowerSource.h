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
   class DcPowerSourceAbc : public SimComponent
   {
      public:
      
      /// @name Lifecycle.
      /// @{
     
         DcPowerSourceAbc(const std::string& id) : SimComponent(id) {}

      /// @}
      
      /// @name DC Power.
      /// @{
         
         virtual double PDc() const = 0;

      /// @}

         Event dcPowerChanged_{"DC power changed"};
   };

   class GenericDcPowerSource : virtual public DcPowerSourceAbc
   {
      public:

      /// @name Lifecycle
      /// @{
      
         GenericDcPowerSource(const std::string& id) : DcPowerSourceAbc(id), PDc_(0.0) {}
      
      /// @}

      /// @name DC Power.
      /// @{
      
         virtual double PDc() const {return PDc_;}
         void setPDc(double PDc) {PDc_ = PDc; needsUpdate().trigger();}
      
      /// @}

      private:
         double PDc_;
   };
}

#endif // DC_POWER_SOURCE_DOT_H
