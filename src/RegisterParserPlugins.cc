#include "RegisterParserPlugins.h"
#include <SmartGridToolbox/Branch.h>
#include <SmartGridToolbox/Bus.h>
#include "CDFParser.h"
#include "MatpowerParser.h"
#include <SmartGridToolbox/Network.h>
#include <SmartGridToolbox/SimpleDCPowerSource.h>
#include <SmartGridToolbox/SimpleInverter.h>
#include <SmartGridToolbox/SolarPV.h>
#include <SmartGridToolbox/Weather.h>
#include <SmartGridToolbox/ZipToGround.h>

namespace SmartGridToolbox
{
   void registerParserPlugins(Parser & p)
   {
      p.registerParserPlugin<BranchParser>();
      p.registerParserPlugin<BusParser>();
      p.registerParserPlugin<CDFParser>();
      p.registerParserPlugin<MatpowerParser>();
      p.registerParserPlugin<NetworkParser>();
      p.registerParserPlugin<SimpleDCPowerSourceParser>();
      p.registerParserPlugin<SimpleInverterParser>();
      p.registerParserPlugin<SolarPVParser>();
      p.registerParserPlugin<WeatherParser>();
      p.registerParserPlugin<ZipToGroundParser>();
   }
}
