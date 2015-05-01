#include "PvDemo.h"

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
    sim.initialize();
    SimNetwork& simNetwork = *sim.simComponent<SimNetwork>("network");
    Network& network = *simNetwork.network();
    // network.setSolver(std::unique_ptr<Sgt::PowerFlowSolverInterface>(new PvDemoSolver));

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
    for (auto gen : network.gens())
    {
        auto inv = std::dynamic_pointer_cast<PvInverter>(gen);
        if (inv)
        {
            invs.push_back(inv.get());
            std::cerr << "Inverter " << inv->id() << std::endl;
        }
    }

    auto sumInv = [&invs]()->Complex {Complex s = 0; for (auto inv : invs) s += inv->S()(0); return s;};
    while (!sim.isFinished())
    {
        Complex S = sumInv();
        double h = dSeconds(sim.currentTime() - sim.startTime()) / 3600.0;
        std::cerr << h << " " << S << std::endl;
        outFile << h << " " << S << std::endl;
        sim.doTimestep();
    }
}
