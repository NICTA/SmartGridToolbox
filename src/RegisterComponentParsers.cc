#include "RegisterComponentParsers.h"
#include "Branch1P.h"
#include "Branch.h"
#include "Bus1P.h"
#include "Bus.h"
#include "Network1P.h"
#include "Network.h"
#include "TestComponent.h"
#include "ZipToGround1P.h"
#include "ZipToGround.h"

namespace SmartGridToolbox
{
   void registerComponentParsers(Parser & p)
   {
      p.registerComponentParser<Branch1PParser>();
      p.registerComponentParser<BranchParser>();
      p.registerComponentParser<Bus1PParser>();
      p.registerComponentParser<BusParser>();
      p.registerComponentParser<Network1PParser>();
      p.registerComponentParser<NetworkParser>();
      p.registerComponentParser<TestComponentParser>();
      p.registerComponentParser<ZipToGround1PParser>();
      p.registerComponentParser<ZipToGroundParser>();
   }
}
