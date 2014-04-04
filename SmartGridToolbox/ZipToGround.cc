#include <SmartGridToolbox/ZipToGround.h>
#include <SmartGridToolbox/Bus.h>
#include <SmartGridToolbox/Model.h>

namespace SmartGridToolbox
{
   ZipToGround::ZipToGround(const std::string & name, const Phases & phases) :
      ZipToGroundBase(name, phases),
      Y_(phases.size(), 0.0),
      I_(phases.size(), 0.0),
      S_(phases.size(), 0.0)
   {
      // Empty.
   }

}
