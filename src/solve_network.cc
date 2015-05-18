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

    assert(argc == 4);
    std::string solver = argv[1];
    std::string inFName = argv[2];
    std::string outPrefix = argv[3];
    Network nw("network", 100.0);

    std::string yamlStr = std::string("--- [{power_flow_solver : " + solver + "}, {matpower : {input_file : ") +
        inFName + ", default_kV_base : 11}}]";
    YAML::Node n = YAML::Load(yamlStr);

    Sgt::Parser<Network> p;
    p.parse(n, nw);
    auto outFBus = std::fopen((outPrefix + ".bus").c_str(), "w+");
    auto outFBranch = std::fopen((outPrefix + ".branch").c_str(), "w+");
    std::map<std::string, int> busMap;

    auto print = [&]()
    {
        printf("--------------------------------------------------------------------------\n");
        printf("%18s : %15s %15s %15s %15s %15s %15s %15s %15s %15s %15s %15s\n",
               "bus", "V_base", "|V|", "theta", "P_gen", "Q_gen", "P_load", "Q_load",
               "y_shunt_r", "y_shunt_i", "I_zip_r", "I_zip_i");
        printf("--------------------------------------------------------------------------\n");

        int iBus = 1;
        Complex SGenTot = 0;
        Complex SLoadTot = 0;
        double costTot = 0;

        for (auto bus : nw.busses())
        {
            busMap[bus->id()] = iBus;
            auto V = nw.V2Pu(bus->V()[0], bus->VBase());
            auto SGen = bus->SGen()[0];
            SGenTot += SGen;
            auto SLoad = -bus->SConst()[0];
            SLoadTot += SLoad;
            auto yZip = bus->YConst()[0];
            auto IConst = bus->IConst()[0];
            printf("%18s : %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f\n",
                   bus->id().c_str(), bus->VBase(), std::abs(V), std::arg(V)*180.0/pi,
                   SGen.real(), SGen.imag(), SLoad.real(), SLoad.imag(),
                   yZip.real(), yZip.imag(), IConst.real(), IConst.imag());
            fprintf(outFBus,
                    "%15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f\n",
                    bus->VBase(), std::abs(V), std::arg(V)*180.0/pi,
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
            fprintf(outFBranch, "%5d %5d %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f\n",
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

    std::cout << "Before:" << std::endl;
    print();
    nw.solvePowerFlow();
    std::cout << "After:" << std::endl;
    print();
    fclose(outFBus);
    fclose(outFBranch);
}
