#include "RegisterParserPlugins.h"
#include "Branch.h"
#include "Bus.h"
#include "Network.h"
#include "SimpleDCPowerSource.h"
#include "SimpleInverter.h"
#include "SolarPV.h"
#include "Weather.h"
#include "ZipToGround.h"

namespace SmartGridToolbox
{
   void registerParserPlugins(Parser & p)
   {
      p.registerParserPlugin<BranchParser>();
      p.registerParserPlugin<BusParser>();
      p.registerParserPlugin<NetworkParser>();
      p.registerParserPlugin<SimpleDCPowerSourceParser>();
      p.registerParserPlugin<SimpleInverterParser>();
      p.registerParserPlugin<SolarPVParser>();
      p.registerParserPlugin<WeatherParser>();
      p.registerParserPlugin<ZipToGroundParser>();
   }
}
