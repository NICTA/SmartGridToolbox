#include "Zip.h"

#include <ostream>

namespace SmartGridToolbox
{
   void ZipInterface::print(std::ostream& os) const
   {
      ComponentInterface::print(os);
      IndentingOStreamBuf _(os);
      os << "id: " << id() << std::endl;
      os << "phases: " << phases() << std::endl;
      os << "YConst: " << YConst() << std::endl;
      os << "IConst: " << IConst() << std::endl;
      os << "SConst: " << SConst() << std::endl;
   }

   GenericZip::GenericZip(const std::string& id, Phases phases) :
      ZipAbc(id, phases),
      YConst_(phases.size(), czero), IConst_(phases.size(), czero), SConst_(phases.size(), czero)
   {
      // Empty.
   }
}
