#include <SgtCore/Parser.h>

#include "BatteryParser.h"
#include "SimBusParser.h"
#include "SimDgyTransformerParser.h"
#include "SimGenericBranchParser.h"
#include "SimNetworkParser.h"
#include "SimOverheadLineParser.h"
#include "SimpleBuildingParser.h"
#include "SimpleDcPowerSourceParser.h"
#include "SimpleInverterParser.h"
#include "Simulation.h"
#include "SimZipParser.h"
#include "SinglePhaseTransformerParser.h"
#include "SolarPvParser.h"
#include "WeatherParser.h"
#include "YyTransformerParser.h"

namespace SmartGridToolbox
{
   void registerParserPlugins(Parser<Simulation>& p)
   {
      p.registerParserPlugin<BatteryParser>();
      p.registerParserPlugin<SimBusParser>();
      p.registerParserPlugin<DgyTransformerParser>();
      p.registerParserPlugin<SimGenericBranchParser>();
      p.registerParserPlugin<SimNetworkParser>();
      p.registerParserPlugin<OverheadLineParser>();
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
