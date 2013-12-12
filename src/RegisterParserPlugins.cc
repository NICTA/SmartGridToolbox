#include "RegisterParserPlugins.h"
#include <SmartGridToolbox/Branch.h>
#include <SmartGridToolbox/Bus.h>
#include "CdfParser.h"
#include "MatpowerParser.h"
#include <SmartGridToolbox/Network.h>
#include <SmartGridToolbox/SimpleBattery.h>
#include <SmartGridToolbox/SimpleBuilding.h>
#include <SmartGridToolbox/SimpleDcPowerSource.h>
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
      p.registerParserPlugin<CdfParser>();
      p.registerParserPlugin<MatpowerParser>();
      p.registerParserPlugin<NetworkParser>();
      p.registerParserPlugin<SimpleBatteryParser>();
      p.registerParserPlugin<SimpleBuildingParser>();
      p.registerParserPlugin<SimpleDcPowerSourceParser>();
      p.registerParserPlugin<SimpleInverterParser>();
      p.registerParserPlugin<SolarPvParser>();
      p.registerParserPlugin<WeatherParser>();
      p.registerParserPlugin<ZipToGroundParser>();
   }
}
