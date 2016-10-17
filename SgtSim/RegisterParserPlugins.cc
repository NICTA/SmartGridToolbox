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
#include "InverterParserPlugin.h"
#include "SimBranchParserPlugin.h"
#include "SimBusParserPlugin.h"
#include "SimGenParserPlugin.h"
#include "SimGlobalParserPlugin.h"
#include "SimMatpowerParserPlugin.h"
#include "SimNetworkParserPlugin.h"
#include "SimpleBuildingParserPlugin.h"
#include "SimZipParserPlugin.h"
#include "TapChangerParserPlugin.h"
#include "TimeSeriesParserPlugin.h"
#include "TimeSeriesZipParserPlugin.h"
#include "Simulation.h"
#include "SolarPvParserPlugin.h"
#include "WeatherParserPlugin.h"

namespace Sgt
{
    class Simulation;
    template<> void registerParserPlugins<Simulation>(Parser<Simulation>& p)
    {
        p.registerParserPlugin<BatteryParserPlugin>();
        p.registerParserPlugin<HeartbeatParserPlugin>();
        p.registerParserPlugin<InverterParserPlugin>();
        p.registerParserPlugin<SimBranchParserPlugin>();
        p.registerParserPlugin<SimBusParserPlugin>();
        p.registerParserPlugin<SimGenParserPlugin>();
        p.registerParserPlugin<SimGlobalParserPlugin>();
        p.registerParserPlugin<SimMatpowerParserPlugin>();
        p.registerParserPlugin<SimNetworkParserPlugin>();
        p.registerParserPlugin<SimpleBuildingParserPlugin>();
        p.registerParserPlugin<SimZipParserPlugin>();
        p.registerParserPlugin<TapChangerParserPlugin>();
        p.registerParserPlugin<TimeSeriesParserPlugin>();
        p.registerParserPlugin<TimeSeriesZipParserPlugin>();
        p.registerParserPlugin<SolarPvParserPlugin>();
        p.registerParserPlugin<WeatherParserPlugin>();
    }
}
