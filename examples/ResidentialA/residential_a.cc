#include "Appliance.h"
#include "ApplianceParserPlugin.h"

#include <SgtCore.h>
#include <SgtSim.h>

#include <fstream>

using namespace SmartGridToolbox;

void print(std::ostream& out, std::ostream& outV, double t, const Network& netw)
{
    out << t;
    outV << t;
    for (ConstBusPtr bus : netw.busses())
    {
        int nPhase = bus->phases().size();
        double P = arma::accu(arma::real(bus->SZip()));
        double VRms = std::sqrt(arma::accu(arma::real(bus->V() * arma::conj(bus->V()))) / nPhase) / bus->VBase();
        out << " " << P;
        outV << " " << VRms;
    }
    out << std::endl;
    outV << std::endl;
}

int main(int argc, const char** argv)
{
    if (argc != 3)
    {
        Log().error() << "Usage: " << argv[0] << " config_name out_name" << std::endl;
        error();
    }

    const char* configName = argv[1];
    const char* outName = argv[2];
    std::string outVName = std::string(outName) + ".V";

    std::ofstream out;
    out.open(outName);

    std::ofstream outV;
    outV.open(outVName);

    Simulation sim;
    Parser<Simulation> p;
    p.registerParserPlugin<ApplianceParserPlugin>();
    p.parse(configName, sim);
    sim.initialize();

    std::shared_ptr<Network> netw = sim.simComponent<SimNetwork>("network")->network();

    out << "% t";
    for (auto bus : netw->busses())
    {
        out << " " << bus->id();
    }
    out << std::endl;

    Stopwatch swTot;
    swTot.start();
    double t = dSeconds(sim.currentTime() - sim.startTime())/3600;
    print(out, outV, t, *netw);
    while (!sim.isFinished())
    {
        sim.doTimestep();
        t = dSeconds(sim.currentTime() - sim.startTime())/3600;
        print(out, outV, t, *netw);
    }
    swTot.stop();
    Log().message() << "Total time = " << swTot.seconds() << std::endl;

    out.close();
}
