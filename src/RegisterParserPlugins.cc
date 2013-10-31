#include "RegisterParserPlugins.h"
#include <smartgridtoolbox/Branch.h>
#include <smartgridtoolbox/Bus.h>
#include "CDFParser.h"
#include "MatpowerParser.h"
#include <smartgridtoolbox/Network.h>
#include <smartgridtoolbox/SimpleDCPowerSource.h>
#include <smartgridtoolbox/SimpleInverter.h>
#include <smartgridtoolbox/SolarPV.h>
#include <smartgridtoolbox/Weather.h>
#include <smartgridtoolbox/ZipToGround.h>

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
