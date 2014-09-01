#include <SgtCore/Parser.h>

#include "SimBusParser.h"
#include "SimDgyTransformerParser.h"
#include "SimGenericBranchParser.h"
#include "SimNetworkParser.h"
#include "SimOverheadLineParser.h"
#include "BatteryParser.h"
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
      p.registerParserPlugin<SimGenericBranchParser>();
      p.registerParserPlugin<SimBusParser>();
      p.registerParserPlugin<DgyTransformerParser>();
      p.registerParserPlugin<SimNetworkParser>();
      p.registerParserPlugin<OverheadLineParser>();
      p.registerParserPlugin<BatteryParser>();
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
