// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <SgtCore/Parser.h>

#include "BatteryParserPlugin.h"
#include "HeartbeatParserPlugin.h"
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

namespace Sgt
{
    class Simulation;
    template<> void registerParserPlugins<Simulation>(Parser<Simulation>& p)
    {
        p.registerParserPlugin<BatteryParserPlugin>();
        p.registerParserPlugin<HeartbeatParserPlugin>();
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
