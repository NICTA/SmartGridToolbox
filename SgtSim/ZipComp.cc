#include "SimZip.h"

#include "SimBus.h"
#include "Model.h"

namespace SmartGridToolbox
{
   SimZip::SimZip(const std::string& id, const Phases& phases) :
      ZipBase(id, phases),
      Y_(phases.size(), 0.0),
      I_(phases.size(), 0.0),
      S_(phases.size(), 0.0)
   {
      // Empty.
   }

}
