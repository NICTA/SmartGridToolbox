#include <SgtCore/Parser.h>

#include "BatteryParser.h"
#include "SimBusParser.h"
#include "SimDgyTransformerParser.h"
#include "SimGenericBranchParser.h"
#include "SimGenericZipParser.h"
#include "SimNetworkParser.h"
#include "SimOverheadLineParser.h"
#include "SimpleBuildingParser.h"
#include "InverterParser.h"
#include "Simulation.h"
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
      p.registerParserPlugin<SimDgyTransformerParser>();
      p.registerParserPlugin<SimGenericBranchParser>();
      p.registerParserPlugin<SimGenericZipParser>();
      p.registerParserPlugin<SimNetworkParser>();
      p.registerParserPlugin<OverheadLineParser>();
      p.registerParserPlugin<SimpleBuildingParser>();
      p.registerParserPlugin<InverterParser>();
      p.registerParserPlugin<SinglePhaseTransformerParser>();
      p.registerParserPlugin<SolarPvParser>();
      p.registerParserPlugin<WeatherParser>();
      p.registerParserPlugin<YyTransformerParser>();
   }
}
