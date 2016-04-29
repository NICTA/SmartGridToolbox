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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>

int main(int argc, char** argv)
{
    using namespace Sgt;

    sgtAssert(argc == 6, "Usage: solve_network solver infile out_prefix warm_start debug");

    std::string solver = argv[1];
    std::string inFName = argv[2];
    std::string outPrefix = argv[3];
    bool warm_start = argv[4][0] == 'T';
    bool debug = argv[5][0] == 'T';

    if (debug)
    {
        std::cout << "debug" << std::endl;
        Sgt::messageLogLevel() = Sgt::LogLevel::VERBOSE;
        Sgt::warningLogLevel() = Sgt::LogLevel::VERBOSE;
        Sgt::debugLogLevel() = Sgt::LogLevel::NORMAL;
    }

    Network nw(100.0);

    std::string yamlStr = std::string("--- [{power_flow_solver : " + solver + "}, {matpower : {input_file : ") +
        inFName + ", default_kV_base : 11}}]";
    YAML::Node n = YAML::Load(yamlStr);

    Parser<Network> p;
    p.parse(n, nw);
    auto outFBus = std::fopen((outPrefix + ".bus").c_str(), "w+");
    auto outFBranch = std::fopen((outPrefix + ".branch").c_str(), "w+");
    std::map<std::string, int> busMap;

    nw.setUseFlatStart(!warm_start);

    auto print = [&]()
    {
        fprintf(outFBus, "# ------------------------------------------------------------------------\n");
        fprintf(outFBus,
                "# %14s %11s %11s %11s %11s %11s %11s %11s %11s %11s %11s %11s\n",
                "bus", "V_base", "|V|", "theta", "P_gen", "Q_gen", "P_load", "Q_load",
                "y_shunt_r", "y_shunt_i", "I_zip_r", "I_zip_i");
        fprintf(outFBus, "# ------------------------------------------------------------------------\n");
        
        fprintf(outFBranch, "# ------------------------------------------------------------------------\n");
        fprintf(outFBranch,
                "# %3s %5s %11s %11s %11s %11s %11s %11s %11s %11s\n",
                "b0", "b1", "Y00r", "Y00i", "Y01r", "Y01i", "Y10r", "Y10i", "Y11r", "Y11i");
        fprintf(outFBranch, "# ------------------------------------------------------------------------\n");

        int iBus = 1;
        Complex SGenTot = 0;
        Complex SLoadTot = 0;
        double costTot = 0;

        for (auto bus : nw.buses())
        {
            busMap[bus->id()] = iBus;
            auto V = nw.V2Pu(bus->V()[0], bus->VBase());
            auto SGen = bus->SGen()[0];
            SGenTot += SGen;
            auto SLoad = -bus->SConst()[0];
            SLoadTot += SLoad;
            auto yZip = bus->YConst()[0];
            auto IConst = bus->IConst()[0];
            fprintf(outFBus,
                    "%16s %11.6f %11.6f %11.6f %11.6f %11.6f %11.6f %11.6f %11.6f %11.6f %11.6f %11.6f\n",
                    bus->id().c_str(), bus->VBase(), std::abs(V), std::arg(V)*180.0/pi,
                    SGen.real(), SGen.imag(), SLoad.real(), SLoad.imag(),
                    yZip.real(), yZip.imag(), IConst.real(), IConst.imag());
            ++iBus;
        }

        for (auto branch : nw.branches())
        {
            auto bus0 = branch->bus0();
            auto bus1 = branch->bus1();
            int iBus0 = busMap[bus0->id()];
            int iBus1 = busMap[bus1->id()];
            fprintf(outFBranch, "%5d %5d %11.6f %11.6f %11.6f %11.6f %11.6f %11.6f %11.6f %11.6f\n",
                    iBus0, iBus1, branch->Y()(0, 0).real(), branch->Y()(0, 0).imag(), branch->Y()(0, 1).real(),
                    branch->Y()(0, 1).imag(), branch->Y()(1, 0).real(), branch->Y()(1, 0).imag(),
                    branch->Y()(1, 1).real(), branch->Y()(1, 1).imag());
        }

        for (auto gen : nw.gens())
        {
            costTot += gen->cost();
        }

        std::cout << std::endl;
        std::cout << "Total Generation = " << SGenTot << std::endl;
        std::cout << "Total Load = " << SLoadTot << std::endl;
        std::cout << "Total Cost = " << costTot << std::endl;
    };

    Stopwatch sw;
    sw.start();
    bool success = nw.solvePowerFlow();
    sw.stop();
    print();
    std::cout << "(success, time) = " << success << " " << sw.seconds() << std::endl;
    fclose(outFBus);
    fclose(outFBranch);
}
