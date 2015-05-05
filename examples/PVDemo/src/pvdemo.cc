#include "PvDemoSolver.h"
#include "PvInverter.h"

#include <SgtSim/Simulation.h>
#include <SgtSim/SimNetwork.h>

int main(int argc, const char ** argv)
{
    using namespace Sgt;

    const char * configName = argv[1];
    const char * outputName = argv[2];
    double vlb = atof(argv[3]);
    double vub = atof(argv[4]);

    std::cout << "Configuration filename = " << configName << std::endl;
    std::cout << "Output filename        = " << outputName << std::endl;
    std::cout << "Voltage lower bound    = " << vlb << std::endl;
    std::cout << "Voltage upper bound    = " << vub << std::endl;

    std::ofstream outFile(outputName);

    Simulation sim;
    Parser<Simulation> p;
    p.registerParserPlugin<PvInverterParserPlugin>();
    p.parse(configName, sim);
    SimNetwork& simNetwork = *sim.simComponent<SimNetwork>("network");
    Network& network = *simNetwork.network();
    network.setSolver(std::unique_ptr<Sgt::PowerFlowSolverInterface>(new PvDemoSolver));
    sim.initialize();

    for (auto bus : network.busses())
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

    std::vector<PvInverter*> invs;
    std::vector<GenAbc*> otherGens;
    for (auto gen : network.gens())
    {
        auto inv = std::dynamic_pointer_cast<PvInverter>(gen);
        if (inv != nullptr)
        {
            invs.push_back(inv.get());
        }
        else
        {
            otherGens.push_back(gen.get());
        }
    }

    auto sumLoad = [&] () -> Complex {Complex x = 0; for (auto bus : network.busses()) x -= bus->SZip()(0); return x;};
    auto sumGen = [&] () -> Complex {Complex x = 0; for (auto gen : otherGens) x += gen->S()(0); return x;};
    auto sumInv = [&] () -> Complex {Complex x = 0; for (auto inv : invs) x += inv->S()(0); return x;};
    auto minV = [&] () -> double {
        double minV = 100;
        for (auto bus : network.busses())
        {
            double V = network.V2Pu(std::abs(bus->V()(0)), bus->VBase());
            if (V < minV)
            {
                minV = V;
            }
        }
        return minV;
    };

    auto print = [&] () {
        Complex SLoad = sumLoad();
        Complex SGen = sumGen();
        Complex SInv = sumInv();
        Complex STot = SGen + SInv;
        Complex VMin = minV();
        double h = dSeconds(sim.currentTime() - sim.startTime()) / 3600.0;
        outFile << h << " " << SLoad << " " << SGen << " " << SInv << " " << STot << " " << VMin
                << " " << network.genCostPerUnitTime() << std::endl;
    };

    while (!sim.isFinished())
    {
        print();
        sim.doTimestep();
    }
    print();
}
