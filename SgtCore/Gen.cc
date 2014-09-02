#include "Gen.h"

#include <ostream>

namespace SmartGridToolbox
{
   void GenInterface::print(std::ostream& os) const
   {
      ComponentInterface::print(os);
      IndentingOStreamBuf _(os);
      os << "phases: " << phases() << std::endl;
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

   GenAbc::GenAbc(const std::string& id, Phases phases) :
      Component(id),
      phases_(phases)
   {
      // Empty.
   }

   GenericGen::GenericGen(const std::string& id, Phases phases) :
      GenAbc(id, phases),
      isInService_(true),
      S_(phases.size(), czero)
   {
      // Empty.
   }
}
