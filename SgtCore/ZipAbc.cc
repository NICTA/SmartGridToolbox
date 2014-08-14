#include "ZipAbc.h"

#include <ostream>

namespace SmartGridToolbox
{

   ZipAbc::ZipAbc(const std::string& id, Phases phases) :
      Component(id), phases_(phases)
   {
      // Empty.
   }

   void ZipAbc::print(std::ostream& os) const
   {
      Component::print(os);
      IndentingOStreamBuf _(os);
      os << "id: " << id() << std::endl;
      os << "phases: " << phases_ << std::endl;
      os << "YConst: " << YConst() << std::endl;
      os << "IConst: " << IConst() << std::endl;
      os << "SConst: " << SConst() << std::endl;
   }
}
