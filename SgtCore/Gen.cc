#include "Gen.h"

#include <ostream>

namespace SmartGridToolbox
{
   Gen::Gen(const std::string& id, Phases phases) :
      GenAbc(id, phases),
      status_(true),
      S_(phases.size(), czero)
   {
      // Empty.
   }
}
