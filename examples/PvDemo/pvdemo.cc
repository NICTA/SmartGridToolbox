// Copyright 2015-2016 National ICT Australia Limited (NICTA)
// Copyright 2016-2019 The Australian National University
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

    std::list<SimComponentPtr<PvInverter>> invs;
    for (const auto& simComp : sim.simComponents())
    {
        auto inv = simComp.as<PvInverter>();
        if (inv != nullptr)
        {
            invs.push_back(inv);
        }
    }

    auto solver = make_unique<PvDemoSolver>();
    for (const auto& inv : invs) 
    {
        solver->addPvInverter(inv);
    }

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

    set<string> invIds;
    for (const auto& inv : invs) invIds.insert(inv->gen()->id());
    std::list<ComponentPtr<Sgt::Gen>> otherGens;
    for (const auto& gen : network.gens())
    {
        if (invIds.find(gen->id()) == invIds.end())
        {
            otherGens.push_back(gen);
        }
    }

    for (auto inv : invs)
    {
        if (!useQ)
        {
            inv->gen()->setQMax(0.0);
        }
    }

    auto sumLoad = [&] () {Sgt::Complex x = 0; for (auto bus : network.buses()) x += bus->SZip()(0, 0); return x;};
    auto sumGen = [&] () {Sgt::Complex x = 0; for (auto gen : otherGens) x += gen->S()(0); return x;};
    auto sumInv = [&] () {Sgt::Complex x = 0; for (auto inv : invs) x += inv->gen()->S()(0); return x;};
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
        Sgt::Complex SLoad = sumLoad();
        Sgt::Complex SNormalGen = sumGen();
        Sgt::Complex SInvGen = sumInv();
        Sgt::Complex VMin = minV();
        Sgt::Complex VMax = maxV();
        outFile << h << "\t" << SLoad << "\t" << SNormalGen << "\t" << SInvGen << "\t" 
            << VMin << "\t" << VMax << "\t" << std::endl;
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
