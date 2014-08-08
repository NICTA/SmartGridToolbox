#include <SmartGridToolbox/Zip.h>
#include <SmartGridToolbox/Bus.h>
#include <SmartGridToolbox/Model.h>

namespace SmartGridToolbox
{
   Zip::Zip(const std::string& name, const Phases& phases) :
      ZipBase(name, phases),
      Y_(phases.size(), 0.0),
      I_(phases.size(), 0.0),
      S_(phases.size(), 0.0)
   {
      // Empty.
   }

}
