#ifndef SIMPLE_DC_POWER_SOURCE_DOT_H
#define SIMPLE_DC_POWER_SOURCE_DOT_H

#include <SmartGridToolbox/DcPowerSourceBase.h>

namespace SmartGridToolbox
{
   class SimpleDcPowerSource : public DcPowerSourceBase
   {
      public:
         SimpleDcPowerSource(const std::string & name) : DcPowerSourceBase(name), PDc_(0.0) {}

         void setPDc(double PDc) {PDc_ = PDc; needsUpdate().trigger();}

      // Overridden from DcPowerSourceBase.
      public:
         virtual double PDc() const {return PDc_;}

      public:
         double PDc_;
   };
}

#endif // SIMPLE_DC_POWER_SOURCE_DOT_H
