#include "RegisterComponentParsers.h"
#include "Branch1P.h"
#include "Bus1P.h"
#include "Network1P.h"
#include "TestComponent.h"
#include "ZipToGround1P.h"

namespace SmartGridToolbox
{
   void registerComponentParsers(Parser & p)
   {
      p.registerComponentParser<Branch1PParser>();
      p.registerComponentParser<Bus1PParser>();
      p.registerComponentParser<Network1PParser>();
      p.registerComponentParser<TestComponentParser>();
      p.registerComponentParser<ZipToGround1PParser>();
   }
}
