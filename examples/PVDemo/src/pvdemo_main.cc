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
    for (auto bus : network.busses())
    {
        bus->setVMagMin(vlb * bus->VBase());
        bus->setVMagMax(vub * bus->VBase());
    }
    auto& bus = *sim.simComponent<Sgt::Bus>("bus_6");
    auto& inv = *sim.simComponent<PvInverter>("pv_inverter_6");
    simNetwork.network()->setSolver(std::unique_ptr<Sgt::PowerFlowSolverInterface>(new PvDemoSolver));

    while (!sim.isFinished())
    {
        std::cout << "TIME " << sim.currentTime() << std::endl;
        std::cout 
            << "OUTPUT"
            << " " << dSeconds(sim.currentTime() - sim.startTime()) / 60
            << " " << inv.S()(0).real()
            << " " << inv.S()(0).imag()
            << " " << std::abs(bus.V()(0))/bus.VBase()
            << std::endl;
        LogIndent _;
        sim.doTimestep();
    }
}
