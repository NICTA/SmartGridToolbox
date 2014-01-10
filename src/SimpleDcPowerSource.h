#ifndef SIMPLE_DC_POWER_SOURCE_DOT_H
#define SIMPLE_DC_POWER_SOURCE_DOT_H

#include <SmartGridToolbox/DcPowerSourceBase.h>

namespace SmartGridToolbox
{
   class SimpleDcPowerSource : public DcPowerSourceBase
   {
      /// @name Overridden member functions from DcPowerSourceBase.
      /// @{
      public:
         SimpleDcPowerSource(const std::string & name) : DcPowerSourceBase(name), PDc_(0.0) {}
         virtual double PDc() const {return PDc_;}
      /// @}

      /// @name My public member functions.
      /// @{
      public:
         void setPDc(double PDc) {PDc_ = PDc; needsUpdate().trigger();}
      /// @}
      
      /// @name My private member variables.
      /// @{
      public:
         double PDc_;
   };
}

#endif // SIMPLE_DC_POWER_SOURCE_DOT_H
