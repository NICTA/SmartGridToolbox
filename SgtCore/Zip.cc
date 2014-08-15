#include "Zip.h"

#include <ostream>

namespace SmartGridToolbox
{

   Zip::Zip(const std::string& id, Phases phases) :
      ZipAbc(id, phases),
      YConst_(phases.size(), czero), IConst_(phases.size(), czero), SConst_(phases.size(), czero)
   {
      // Empty.
   }
}
