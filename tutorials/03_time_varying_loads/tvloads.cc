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

#include <SgtSim/Simulation.h>
#include <SgtSim/SimNetwork.h>

int main(int argc, const char ** argv)
{
    using namespace Sgt;

    // messageLogLevel() = LogLevel::VERBOSE;
    // warningLogLevel() = LogLevel::VERBOSE;
    // debugLogLevel() = LogLevel::VERBOSE;

    // const char * configName = argv[1];
    std::string configName = "input.yaml";
    // const char * outputName = argv[2];
    std::string outputName = "out.txt";

    std::cout << "Configuration filename = " << configName << std::endl;
    std::cout << "Output filename        = " << outputName << std::endl;

    std::ofstream outFile(outputName);

    Simulation sim;
    Parser<Simulation> p;
    p.parse(configName, sim);
    SimNetwork& simNetwork = *sim.simComponent<SimNetwork>("network");
    Network& network = simNetwork.network();
    network.setUseFlatStart(true);

    auto sumLoad = [&] () {Complex x = 0; for (auto bus : network.buses()) x -= bus->SZip()(0); return x;};
    auto sumGen = [&] () {Complex x = 0; for (auto gen : network.gens()) x += gen->S()(0); return x;};

    auto minV = [&] () {
        double minV1 = 100;
        for (auto bus : network.buses())
        {
            double V = network.V2Pu(std::abs(bus->V()(0)), bus->VBase());
            if (V < minV1)
            {
                minV1 = V;
            }
        }
        return minV1;
    };
    auto maxV = [&] () {
        double maxV1 = 0;
        for (auto bus : network.buses())
        {
            double V = network.V2Pu(std::abs(bus->V()(0)), bus->VBase());
            if (V > maxV1)
            {
                maxV1 = V;
            }
        }
        return maxV1;
    };

    auto print = [&] () {
        double h = dSeconds(sim.currentTime() - sim.startTime()) / 3600.0;
        outFile << h << " " << sumLoad() << " " << sumGen() << " " << minV() << " " << maxV() << " " << std::endl;
    };

    sim.initialize();
    while (!sim.isFinished())
    {
        std::cout << sim.currentTime()-sim.startTime() << std::endl;
        sim.doTimestep();
        print();
    }
    print();
}
