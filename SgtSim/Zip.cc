#include <SgtSim/ZipComp.h>
#include <SgtSim/BusComp.h>
#include <SgtSim/Model.h>

namespace SmartGridToolbox
{
   ZipComp::ZipComp(const std::string& name, const Phases& phases) :
      ZipBase(name, phases),
      Y_(phases.size(), 0.0),
      I_(phases.size(), 0.0),
      S_(phases.size(), 0.0)
   {
      // Empty.
   }

}
