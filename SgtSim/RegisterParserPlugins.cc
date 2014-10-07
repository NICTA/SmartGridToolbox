#include <SgtCore/Parser.h>

#include "BatteryParserPlugin.h"
#include "SimBusParserPlugin.h"
#include "SimDgyTransformerParserPlugin.h"
#include "SimGenericBranchParserPlugin.h"
#include "SimGenericGenParserPlugin.h"
#include "SimGenericZipParserPlugin.h"
#include "SimGlobalParserPlugin.h"
#include "SimMatpowerParserPlugin.h"
#include "SimNetworkParserPlugin.h"
#include "SimOverheadLineParserPlugin.h"
#include "SimSinglePhaseTransformerParserPlugin.h"
#include "SimpleBuildingParserPlugin.h"
#include "SimYyTransformerParserPlugin.h"
#include "TimeSeriesParserPlugin.h"
#include "TimeSeriesZipParserPlugin.h"
#include "InverterParserPlugin.h"
#include "Simulation.h"
#include "SimSinglePhaseTransformerParserPlugin.h"
#include "SolarPvParserPlugin.h"
#include "WeatherParserPlugin.h"

namespace SmartGridToolbox
{
   class Simulation;
   template<> void registerParserPlugins<Simulation>(Parser<Simulation>& p)
   {
      p.registerParserPlugin<BatteryParserPlugin>();
      p.registerParserPlugin<SimBusParserPlugin>();
      p.registerParserPlugin<SimDgyTransformerParserPlugin>();
      p.registerParserPlugin<SimGenericBranchParserPlugin>();
      p.registerParserPlugin<SimGenericGenParserPlugin>();
      p.registerParserPlugin<SimGenericZipParserPlugin>();
      p.registerParserPlugin<SimGlobalParserPlugin>();
      p.registerParserPlugin<SimMatpowerParserPlugin>();
      p.registerParserPlugin<SimNetworkParserPlugin>();
      p.registerParserPlugin<SimOverheadLineParserPlugin>();
      p.registerParserPlugin<SimpleBuildingParserPlugin>();
      p.registerParserPlugin<SimYyTransformerParserPlugin>();
      p.registerParserPlugin<TimeSeriesParserPlugin>();
      p.registerParserPlugin<TimeSeriesZipParserPlugin>();
      p.registerParserPlugin<InverterParserPlugin>();
      p.registerParserPlugin<SimSinglePhaseTransformerParserPlugin>();
      p.registerParserPlugin<SolarPvParserPlugin>();
      p.registerParserPlugin<WeatherParserPlugin>();
   }
}
