#include "SimZip.h"

#include "SimBus.h"
#include "Model.h"

namespace SmartGridToolbox
{
   SimZip::SimZip(const std::string& name, const Phases& phases) :
      ZipBase(name, phases),
      Y_(phases.size(), 0.0),
      I_(phases.size(), 0.0),
      S_(phases.size(), 0.0)
   {
      // Empty.
   }

}
