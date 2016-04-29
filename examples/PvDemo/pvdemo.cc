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

#include "PvDemoSolver.h"
#include "PvInverter.h"

#include <SgtSim/Simulation.h>
#include <SgtSim/SimNetwork.h>
#include <SgtSim/Weather.h>

int main(int argc, const char ** argv)
{
    using namespace Sgt;

    // messageLogLevel() = LogLevel::VERBOSE;
    // warningLogLevel() = LogLevel::VERBOSE;
    // debugLogLevel() = LogLevel::VERBOSE;

    const char * configName = argv[1];
    const char * outputName = argv[2];
    double vlb = atof(argv[3]);
    double vub = atof(argv[4]);
    bool useQ = argv[5][0] == 'T';

    std::cout << "Configuration filename = " << configName << std::endl;
    std::cout << "Output filename        = " << outputName << std::endl;
    std::cout << "Voltage lower bound    = " << vlb << std::endl;
    std::cout << "Voltage upper bound    = " << vub << std::endl;
    std::cout << "Allow Q                = " << useQ << std::endl;

    std::ofstream outFile(outputName);

    Simulation sim;
    Parser<Simulation> p;
    p.registerParserPlugin<PvInverterParserPlugin>();
    p.parse(configName, sim);
    SimNetwork& simNetwork = *sim.simComponent<SimNetwork>("network");
    Network& network = simNetwork.network();
    network.setSolver(std::unique_ptr<Sgt::PowerFlowSolverInterface>(new PvDemoSolver));
    network.setUseFlatStart(true);

    for (auto bus : network.buses())
    {
        if (bus->type() == BusType::SL || bus->type() == BusType::PV)
        {
            bus->setVMagMin(bus->VMagSetpoint()(0));
            bus->setVMagMax(bus->VMagSetpoint()(0));
        }
        else
        {
            if (vlb > 0)
            {
                bus->setVMagMin(vlb * bus->VBase());
            }
            if (vub > 0)
            {
                bus->setVMagMax(vub * bus->VBase());
            }
        }
    }

    std::vector<PvInverter*> invs;
    std::vector<GenAbc*> otherGens;
    for (auto gen : network.gens())
    {
        auto inv = dynamic_cast<PvInverter*>(gen);
        if (inv != nullptr)
        {
            if (!useQ)
            {
                inv->setMaxQ(0.0);
            }
            invs.push_back(inv);
        }
        else
        {
            otherGens.push_back(gen);
        }
    }

    auto sumLoad = [&] () {Complex x = 0; for (auto bus : network.buses()) x -= bus->SZip()(0); return x;};
    auto sumGen = [&] () {Complex x = 0; for (auto gen : otherGens) x += gen->S()(0); return x;};
    auto sumInv = [&] () {Complex x = 0; for (auto inv : invs) x += inv->gen().S()(0); return x;};
    auto minV = [&] () {
        double minV = 100;
        for (auto bus : network.buses())
        {
            double V = network.V2Pu(std::abs(bus->V()(0)), bus->VBase());
            if (V < minV)
            {
                minV = V;
            }
        }
        return minV;
    };
    auto maxV = [&] () {
        double maxV = 0;
        for (auto bus : network.buses())
        {
            double V = network.V2Pu(std::abs(bus->V()(0)), bus->VBase());
            if (V > maxV)
            {
                maxV = V;
            }
        }
        return maxV;
    };

    auto print = [&] () {
        double h = dSeconds(sim.currentTime() - sim.startTime()) / 3600.0;
        Complex SLoad = sumLoad();
        Complex SNormalGen = sumGen();
        Complex SInvGen = sumInv();
        Complex VMin = minV();
        Complex VMax = maxV();
        outFile << h << " " << SLoad << " " << SNormalGen << " " << SInvGen << " " 
            << VMin << " " << VMax << " " << std::endl;
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
