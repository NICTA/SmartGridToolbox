#include <Common.h>

namespace SmartGridToolbox
{
   Complex complexFromString(const std::string & s)
   {
      // TODO: this implementation is no good because it doesn't handle ill-formed strings.
      std::istringstream ss(s);
      Complex c;
      ss >> c;
      return c;
   }
}
