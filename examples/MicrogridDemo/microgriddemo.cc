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
#include <SgtSim/SimpleBuilding.h>

#include <fstream>

using namespace Sgt;

int main(int argc, const char ** argv)
{
    messageLogLevel() = LogLevel::VERBOSE;
    debugLogLevel() = LogLevel::NORMAL;
    std::string configName(argv[1]);
    std::string outputPrefix(argv[2]);

    std::cout << "Configuration filename = " << configName << std::endl;
    std::cout << "Output prefix          = " << outputPrefix << std::endl;

    std::ofstream VFile(outputPrefix + ".V");
    std::ofstream SFile(outputPrefix + ".S");

    Simulation sim;
    Parser<Simulation> p;
    p.registerParserPlugin<MicrogridControllerParserPlugin>();
    p.parse(configName, sim);

    sim.initialize();


    auto batt = sim.simComponent<Battery>("batt");
    auto inv = sim.simComponent<InverterAbc>("inverter_batt");
    auto simNetw = sim.simComponent<SimNetwork>("network");

    auto busses = simNetw->network()->busses();
    sgtLogMessage() << *simNetw->network() << std::endl;
    while (!sim.isFinished())
    {
        VFile << (dSeconds(sim.currentTime() - sim.startTime())) << " ";
        for (auto & bus : busses)
        {
            VFile <<  bus->V()(0)  << " ";
        }
        VFile << std::endl;

        SFile << (dSeconds(sim.currentTime() - sim.startTime())) << " ";
        for (auto & bus : busses)
        {
            SFile <<  bus->SZip()(0)  << " ";
        }
        SFile << std::endl;

        sim.doTimestep();
    }

    VFile.close();
    SFile.close();
}
