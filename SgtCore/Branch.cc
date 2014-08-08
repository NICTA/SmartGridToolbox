#include "Branch.h"
#include "Bus.h"

#include <ostream>

namespace SmartGridToolbox
{
   std::ostream& Branch::print(std::ostream& os) const
   {
      IndentingOStreamBuf ind(os, "");
      os << "branch: " << std::endl;
      ind.setInd("    ");
      os << "id: " << id() << std::endl;
      os << "phases0: " << phases0() << std::endl;
      os << "phases1: " << phases1() << std::endl;
      os << "bus0: " << bus0().id() << std::endl;
      os << "bus1: " << bus1().id() << std::endl;
      return os;
   }
}
