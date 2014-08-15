#include "GenAbc.h"

#include <ostream>

namespace SmartGridToolbox
{
         
   GenAbc::GenAbc(const std::string& id, Phases phases) :
      Component(id),
      phases_(phases)
   {
      // Empty.
   }

   void GenAbc::print(std::ostream& os) const
   {
      Component::print(os);
      IndentingOStreamBuf _(os);
      os << "phases: " << phases_ << std::endl;
      os << "S: " << S() << std::endl;
      os << "PMin: " << PMin() << std::endl;
      os << "PMax: " << PMax() << std::endl;
      os << "QMin: " << QMin() << std::endl;
      os << "QMax: " << QMax() << std::endl;
      os << "cStartup : " << cStartup() << std::endl;
      os << "cShutdown: " << cShutdown() << std::endl;
      os << "c0: " << c0() << std::endl;
      os << "c1: " << c1() << std::endl;
      os << "c2: " << c2() << std::endl;
   }
}
