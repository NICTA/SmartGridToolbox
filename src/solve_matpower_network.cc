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

#include <SgtCore.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>

char* getCmdOption(char** argv, int argc, const std::string & option)
{
    auto begin = argv;
    auto end = argv + argc;
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** argv, int argc, const std::string& option)
{
    auto begin = argv;
    auto end = argv + argc;
    return std::find(begin, end, option) != end;
}

int main(int argc, char** argv)
{
    using namespace Sgt;

    sgtAssert(argc > 1,
            "Usage: solve_network [--solver solver_arg] [--prefix prefix] [--warm_start] [--debug] infile");

    std::string inFName = argv[argc - 1];

    std::string solver = "nr";
    {
        const char* opt = getCmdOption(argv, argc, "--solver");
        if (opt != nullptr) solver = opt;
    }

    std::string outPrefix = "out";
    {
        const char* opt = getCmdOption(argv, argc, "--prefix");
        if (opt != nullptr) outPrefix = opt;
    }

    bool warmStart = cmdOptionExists(argv, argc, "--warm_start");
    
    const char* debug = getCmdOption(argv, argc, "--debug");

    if (debug)
    {
        std::cout << "debug" << std::endl;
        Sgt::messageLogLevel() = Sgt::LogLevel::VERBOSE;
        Sgt::warningLogLevel() = Sgt::LogLevel::VERBOSE;
        Sgt::debugLogLevel() = std::string(debug) == std::string("verbose") 
            ? Sgt::LogLevel::VERBOSE : Sgt::LogLevel::NORMAL;
    }

    Network nw(100.0);
    Parser<Network> p;

    std::string yamlStr = std::string("--- [{power_flow_solver : " + solver + "}, {matpower : {input_file : ") +
        inFName + ", default_kV_base : 11}}]";
    YAML::Node n = YAML::Load(yamlStr);
    p.parse(n, nw);

    auto outFBus = std::fopen((outPrefix + ".bus").c_str(), "w+");
    auto outFBranch = std::fopen((outPrefix + ".branch").c_str(), "w+");
    std::map<std::string, int> busMap;

    nw.setUseFlatStart(!warmStart);

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
            auto SLoad = bus->SConst()[0];
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
    std::cout << "(success, time) = " << success << " " << sw.cpuSeconds() << std::endl;
    fclose(outFBus);
    fclose(outFBranch);
}
