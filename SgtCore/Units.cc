#include <SgtCore/Units.h>

namespace Units
{
   namespace SI
   {
      // Base SI units.
      const Unit<decltype(LDim())> m = {{LDim(), 1.0}, "m"};
      const Unit<decltype(MDim())> kg = {{MDim(), 1.0}, "kg"};
      const Unit<decltype(TDim())> s = {{TDim(), 1.0}, "s"};
      const Unit<decltype(IDim())> A = {{IDim(), 1.0}, "A"};
      const Unit<decltype(ThDim())> K = {{ThDim(), 1.0}, "K"};

      const Unit<decltype(QDim())> C = {{IDim() * TDim(), 1.0}, "C"};
   }
}
