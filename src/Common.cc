#include <Common.h>
#include <sstream>

namespace SmartGridToolbox
{
   Complex string2Complex(const std::string & s)
   {
      // TODO: this implementation is no good because it doesn't handle ill-formed strings.
      std::istringstream ss(s);
      Complex c;
      ss >> c;
      return c;
   }

   std::string complex2String(Complex c)
   {
      std::ostringstream ss;
      ss << c;
      return ss.str();
   }
}
