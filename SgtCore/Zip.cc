#include "Zip.h"

#include <ostream>

namespace SmartGridToolbox
{

   Zip::Zip(const std::string& id, Phases phases) :
      Component(id), phases_(phases), 
      YConst_(phases.size(), czero), IConst_(phases.size(), czero), SConst_(phases.size(), czero)
   {
      // Empty.
   }

   std::ostream& Zip::print(std::ostream& os) const
   {
      os << "zip:" << std::endl;
      IndentingOStreamBuf _(os);
      os << "id: " << id() << std::endl;
      os << "phases: " << phases_ << std::endl;
      os << "YConst: " << YConst_ << std::endl;
      os << "IConst: " << IConst_ << std::endl;
      os << "SConst: " << SConst_ << std::endl;
      return os;
   }
}
