#include "Branch.h"
#include "Bus.h"

#include <ostream>

namespace SmartGridToolbox
{
         
   Branch::Branch(const std::string& id, Phases phases0, Phases phases1) :
      Component(id),
      phases0_(phases0),
      phases1_(phases1),
      isInService_(true)
   {
      // Empty.
   }
   void Branch::print(std::ostream& os) const
   {
      Component::print(os);
      IndentingOStreamBuf _(os);
      os << "phases0: " << phases0() << std::endl;
      os << "phases1: " << phases1() << std::endl;
      os << "bus0: " << bus0().id() << std::endl;
      os << "bus1: " << bus1().id() << std::endl;
   }
}
