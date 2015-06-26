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

#include <SgtCore.h>
#include <SgtSim.h>

#include <fstream>

using namespace Sgt;

void print(std::ostream& out, std::ostream& outV, double t, const Network& netw)
{
    out << t;
    outV << t;
    for (ConstBusPtr bus : netw.busses())
    {
        auto nPhase = bus->phases().size();
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
    Sgt::Log::messageLevel = Sgt::LogLevel::VERBOSE;
    sgtAssert(argc != 3, "Usage: " << argv[0] << " config_name out_name.");

    const char* configName = argv[1];
    const char* outName = argv[2];
    std::string outVName = std::string(outName) + ".V";

    std::ofstream out;
    out.open(outName);

    std::ofstream outV;
    outV.open(outVName);

    Simulation sim;
    Parser<Simulation> p;
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
