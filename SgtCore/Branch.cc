#include "Branch.h"

#include <ostream>

namespace SmartGridToolbox
{
   void BranchInterface::print(std::ostream& os) const
   {
      ComponentInterface::print(os);
      IndentingOStreamBuf _(os);
      os << "phases0: " << phases0() << std::endl;
      os << "phases1: " << phases1() << std::endl;
   }
         
   BranchAbc::BranchAbc(const std::string& id, Phases phases0, Phases phases1) :
      Component(id),
      phases0_(phases0),
      phases1_(phases1),
      isInService_(true)
   {
      // Empty.
   }
}
