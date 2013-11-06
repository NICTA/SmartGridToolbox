#ifndef DC_POWER_SOURCE_BASE_DOT_H
#define DC_POWER_SOURCE_BASE_DOT_H

#include <SmartGridToolbox/Component.h>

namespace SmartGridToolbox
{
   class DCPowerSourceBase : public Component
   {
      public:
         DCPowerSourceBase(const std::string & name) : Component(name) {}
         virtual double PDC() const = 0;
   };
}

#endif // DC_POWER_SOURCE_BASE_DOT_H
