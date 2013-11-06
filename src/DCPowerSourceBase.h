#ifndef DC_POWER_SOURCE_BASE_DOT_H
#define DC_POWER_SOURCE_BASE_DOT_H

#include <SmartGridToolbox/Component.h>

namespace SmartGridToolbox
{
   class DcPowerSourceBase : public Component
   {
      public:
         DcPowerSourceBase(const std::string & name) : Component(name) {}
         virtual double PDc() const = 0;
   };
}

#endif // DC_POWER_SOURCE_BASE_DOT_H
