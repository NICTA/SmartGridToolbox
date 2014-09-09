#include <SgtCore/Parser.h>

#include "BatteryParser.h"
#include "SimBusParser.h"
#include "SimDgyTransformerParser.h"
#include "SimGenericBranchParser.h"
#include "SimGenericZipParser.h"
#include "SimNetworkParser.h"
#include "SimOverheadLineParser.h"
#include "SimSinglePhaseTransformerParser.h"
#include "SimpleBuildingParser.h"
#include "InverterParser.h"
#include "Simulation.h"
#include "SimSinglePhaseTransformerParser.h"
#include "SolarPvParser.h"
#include "WeatherParser.h"

namespace SmartGridToolbox
{
   class Simulation;
   template<> void registerParserPlugins<Simulation>(Parser<Simulation>& p)
   {
      p.registerParserPlugin<BatteryParser>();
      p.registerParserPlugin<SimBusParser>();
      p.registerParserPlugin<SimDgyTransformerParser>();
      p.registerParserPlugin<SimGenericBranchParser>();
      p.registerParserPlugin<SimGenericZipParser>();
      p.registerParserPlugin<SimNetworkParser>();
      p.registerParserPlugin<SimOverheadLineParser>();
      p.registerParserPlugin<SimpleBuildingParser>();
      p.registerParserPlugin<InverterParser>();
      p.registerParserPlugin<SimSinglePhaseTransformerParser>();
      p.registerParserPlugin<SolarPvParser>();
      p.registerParserPlugin<WeatherParser>();
   }
}
