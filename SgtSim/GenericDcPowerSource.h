#ifndef DC_POWER_SOURCE_DOT_H
#define DC_POWER_SOURCE_DOT_H

#include <SgtSim/DcPowerSourceBase.h>

namespace SmartGridToolbox
{
   class DcPowerSource : public DcPowerSourceBase
   {
      /// @name Overridden member functions from SimComponent.
      /// @{
      
      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;
      
      /// @}
      
      /// @name Overridden member functions from DcPowerSourceBase.
      /// @{
      
      public:
         virtual double PDc() const {return PDc_;}
      
      /// @}
      
      /// @name My public member functions.
      /// @{
      
      public:
         DcPowerSource(const std::string& id) : DcPowerSourceBase(id), PDc_(0.0) {}

         void setPDc(double PDc) {PDc_ = PDc; needsUpdate().trigger();}

      /// @}

      public:
         double PDc_;
   };
}

#endif // DC_POWER_SOURCE_DOT_H
