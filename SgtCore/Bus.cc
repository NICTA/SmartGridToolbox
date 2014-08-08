#include "Bus.h"

#include <ostream>

namespace SmartGridToolbox
{
   std::ostream& Bus::print(std::ostream& os) const
   {
      IndentingOStreamBuf ind(os, "");
      os << "bus:" << std::endl;
      ind.setInd("    ");
      os << "id: " << id_ << std::endl;
      os << "phases: " << phases_ << std::endl;
      os << "type: " << type_ << std::endl;
      os << "V_base: " << VBase_ << std::endl;
      os << "V_nom: " << VNom_ << std::endl;
      os << "V_mag_min: " << VMagMin_ << std::endl;
      os << "V_mag_max: " << VMagMax_ << std::endl;
      os << "V: " << V_ << std::endl;
      os << "zips:" << std::endl;
      ind.setInd("        ");
      for (auto& zip : zips_)
      {
         os << *zip << std::endl;
      }
      ind.setInd("    ");
      os << "gens:" << std::endl;
      ind.setInd("        ");
      for (auto& gen : gens_)
      {
         os << *gen << std::endl;
      }
      return os;
   }
}
