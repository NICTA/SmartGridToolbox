#include "Network.h"

namespace LibPowerFlow
{
   std::ostream& Network::print(std::ostream& os) const
   {
      IndentingOStreamBuf ind(os, "");
      os << "network:" << std::endl;
      ind.setInd("    ");
      os << "P_base: " << PBase_ << std::endl;
      for (auto& bus : busVec_)
      {
         os << *bus << std::endl;
      }
      for (auto& branch : branchVec_)
      {
         os << *branch << std::endl;
      }
      return os;
   }
}
