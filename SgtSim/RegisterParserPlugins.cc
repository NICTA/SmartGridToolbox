#include "RegisterParserPlugins.h"

#include "SimBranchParser.h"
#include "SimBusParser.h"
#include "DgyTransformerParser.h"
#include "SimNetworkParser.h"
#include "OverheadLineParser.h"
#include "SimpleBatteryParser.h"
#include "SimpleBuildingParser.h"
#include "SimpleDcPowerSourceParser.h"
#include "SimpleInverterParser.h"
#include "SinglePhaseTransformerParser.h"
#include "SolarPvParser.h"
#include "WeatherParser.h"
#include "YyTransformerParser.h"
#include "SimZipParser.h"

namespace SmartGridToolbox
{
   void registerParserPlugins(Parser& p)
   {
      p.registerParserPlugin<SimBranchParser>();
      p.registerParserPlugin<SimBusParser>();
      p.registerParserPlugin<DgyTransformerParser>();
      p.registerParserPlugin<SimNetworkParser>();
      p.registerParserPlugin<OverheadLineParser>();
      p.registerParserPlugin<SimpleBatteryParser>();
      p.registerParserPlugin<SimpleBuildingParser>();
      p.registerParserPlugin<SimpleDcPowerSourceParser>();
      p.registerParserPlugin<SimpleInverterParser>();
      p.registerParserPlugin<SinglePhaseTransformerParser>();
      p.registerParserPlugin<SolarPvParser>();
      p.registerParserPlugin<WeatherParser>();
      p.registerParserPlugin<YyTransformerParser>();
      p.registerParserPlugin<SimZipParser>();
   }
}
