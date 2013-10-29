#ifndef DC_POWER_SOURCE_BASE
#define DC_POWER_SOURCE_BASE

#include <smartgridtoolbox/Component.h>

namespace SmartGridToolbox
{
   class DCPowerSourceBase : public Component
   {
      public:
         DCPowerSourceBase(const std::string & name) : Component(name) {}
         virtual double PDC() const = 0;
   };
}

#endif // DC_POWER_SOURCE_BASE
