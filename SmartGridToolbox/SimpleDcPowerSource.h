#ifndef SIMPLE_DC_POWER_SOURCE_DOT_H
#define SIMPLE_DC_POWER_SOURCE_DOT_H

#include <SmartGridToolbox/DcPowerSourceBase.h>

namespace SmartGridToolbox
{
   class SimpleDcPowerSource : public DcPowerSourceBase
   {
      /// @name Overridden member functions from Component.
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
         SimpleDcPowerSource(const std::string& name) : DcPowerSourceBase(name), PDc_(0.0) {}

         void setPDc(double PDc) {PDc_ = PDc; needsUpdate().trigger();}

      /// @}

      public:
         double PDc_;
   };
}

#endif // SIMPLE_DC_POWER_SOURCE_DOT_H
