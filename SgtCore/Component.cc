#include "Component.h"

#include <ostream>

namespace SmartGridToolbox
{
   void Component::print(std::ostream& os) const
   {
      os << componentTypeStr() << ": " << id_ << ":" << std::endl;
   }
}
