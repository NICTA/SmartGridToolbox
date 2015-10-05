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

#include "Building.h"
#include "BuildingController.h"

#include <SgtSim/Battery.h>
#include <SgtSim/Inverter.h>
#include <SgtSim/Simulation.h>
#include <SgtSim/SimGen.h>
#include <SgtSim/TimeSeriesZip.h>

#include <fstream>

using namespace Sgt;

int main(int argc, const char ** argv)
{
    messageLogLevel() = LogLevel::NORMAL;
    // debugLogLevel() = LogLevel::NORMAL;
    std::string configName(argv[1]);
    std::string outName(argv[2]);

    sgtLogMessage() << "Configuration filename = " << configName << std::endl;
    sgtLogMessage() << "Output file            = " << outName << std::endl;

    std::ofstream datFile(outName);

    Simulation sim;
    Parser<Simulation> p;
    p.registerParserPlugin<BuildingControllerParserPlugin>();
    p.registerParserPlugin<BuildingParserPlugin>();
    p.parse(configName, sim);

    sim.initialize();

    auto price = sim.timeSeries<BuildingController::PriceSeries>("price");
    auto genTrans = sim.simComponent<SimGen>("gen_trans");
    auto buildingLoad = sim.simComponent<TimeSeriesZip>("load_build");
    auto building = sim.simComponent<Building>("build");
    auto pvInverter = sim.simComponent<SimpleZipInverter>("pv_inverter");
    auto batt = sim.simComponent<Battery>("battery");
    auto battInv = sim.simComponent<SimpleZipInverter>("battery_inverter");

    while (!sim.isFinished())
    {
        datFile 
            << (dSeconds(sim.currentTime() - sim.startTime()) / 3600.0) << " "  // 1: Time
            << price->value(sim.currentTime()) << " "                           // 2: Price
            << real(buildingLoad->zip()->SConst()(0)) << " "                    // 3: Load uncontrolled inj 
            << real(pvInverter->zip()->SConst()(0)) << " "                      // 4: Pv uncontrolled inj
            << real(genTrans->gen()->S()(0)) << " "                             // 5: Grid inj
            << building->Te() << " "                                            // 6: Extern temp 
            << real(building->zip()->SConst()(0)) << " "                        // 7: Building HVAC inj
            << building->Tb() << " "                                            // 8: Building temp 
            << real(battInv->zip()->SConst()(0)) << " "                         // 9: Battery inj 
            << batt->charge() << std::endl;                                     // 10: Battery charge
        sim.doTimestep();
    }
}
