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

#include "MicrogridController.h"

#include <SgtSim/Battery.h>
#include <SgtSim/Inverter.h>
#include <SgtSim/Simulation.h>
#include <SgtSim/TimeSeriesZip.h>

#include <fstream>

using namespace Sgt;

int main(int argc, const char ** argv)
{
    messageLogLevel() = LogLevel::VERBOSE;
    debugLogLevel() = LogLevel::NORMAL;
    std::string configName(argv[1]);
    std::string outputPrefix(argv[2]);

    sgtLogMessage() << "Configuration filename = " << configName << std::endl;
    sgtLogMessage() << "Output prefix          = " << outputPrefix << std::endl;

    std::ofstream battFile(outputPrefix + ".batt");

    Simulation sim;
    Parser<Simulation> p;
    p.registerParserPlugin<MicrogridControllerParserPlugin>();
    p.parse(configName, sim);

    sim.initialize();

    auto batt = sim.simComponent<Battery>("battery");
    auto inv = sim.simComponent<SimpleZipInverter>("inverter_battery");
    auto buildZip = sim.simComponent<TimeSeriesZip>("load_build");

    while (!sim.isFinished())
    {
        std::cout << "TEST " << buildZip->zip()->SConst() << std::endl;
        battFile << (dSeconds(sim.currentTime() - sim.startTime()) / 3600.0)
            << " " << batt->PDc() << " " << batt->charge() << " " << real(inv->zip()->SConst()(0)) << " "
            << real(buildZip->zip()->SConst()(0)) << std::endl;
        sim.doTimestep();
    }
}
