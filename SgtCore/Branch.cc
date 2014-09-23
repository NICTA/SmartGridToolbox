#include "Branch.h"

#include <ostream>

namespace SmartGridToolbox
{
   BranchAbc::BranchAbc(const std::string& id, const Phases& phases0, const Phases& phases1) :
      Component(id),
      phases0_(phases0),
      phases1_(phases1),
      isInService_(true)
   {
      // Empty.
   }
   
   void BranchAbc::print(std::ostream& os) const
   {
      Component::print(os);
      Indent _;
      os << "phases0: " << phases0() << std::endl;
      os << "phases1: " << phases1() << std::endl;
   }
}
