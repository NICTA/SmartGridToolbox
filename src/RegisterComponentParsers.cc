#include "RegisterComponentParsers.h"
#include "Branch.h"
#include "Bus.h"
#include "Network.h"
#include "SimpleDCPowerSource.h"
#include "SimpleInverter.h"
#include "SolarPV.h"
#include "TestComponent.h"
#include "Weather.h"
#include "ZipToGround.h"

namespace SmartGridToolbox
{
   void registerComponentParsers(Parser & p)
   {
      p.registerComponentParser<BranchParser>();
      p.registerComponentParser<BusParser>();
      p.registerComponentParser<NetworkParser>();
      p.registerComponentParser<SimpleDCPowerSourceParser>();
      p.registerComponentParser<SimpleInverterParser>();
      p.registerComponentParser<SolarPVParser>();
      p.registerComponentParser<TestComponentParser>();
      p.registerComponentParser<WeatherParser>();
      p.registerComponentParser<ZipToGroundParser>();
   }
}
