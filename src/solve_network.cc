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
#include <iomanip>
#include <iostream>
#include <map>

using namespace arma;
using namespace std;

char* getCmdOption(char** argv, int argc, const string & option)
{
    auto begin = argv;
    auto end = argv + argc;
    char ** itr = find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** argv, int argc, const string& option)
{
    auto begin = argv;
    auto end = argv + argc;
    return find(begin, end, option) != end;
}

int main(int argc, char** argv)
{
    using namespace Sgt;

    sgtAssert(argc > 1,
            "Usage: solve_network [--solver solver_arg] [--prefix prefix] [--warm_start] [--debug] infile");

    string inFName = argv[argc - 1];

    string solver = "nr_rect";
    {
        const char* opt = getCmdOption(argv, argc, "--solver");
        if (opt != nullptr) solver = opt;
    }

    string outPrefix = "out";
    {
        const char* opt = getCmdOption(argv, argc, "--prefix");
        if (opt != nullptr) outPrefix = opt;
    }

    bool warmStart = cmdOptionExists(argv, argc, "--warm_start");
    
    const char* debug = getCmdOption(argv, argc, "--debug");

    if (debug)
    {
        cout << "debug" << endl;
        Sgt::messageLogLevel() = Sgt::LogLevel::VERBOSE;
        Sgt::warningLogLevel() = Sgt::LogLevel::VERBOSE;
        Sgt::debugLogLevel() = string(debug) == string("verbose") 
            ? Sgt::LogLevel::VERBOSE : Sgt::LogLevel::NORMAL;
    }

    Network nw(100.0);
    Parser<Network> p;
    p.parse(inFName, nw);
    if (solver == "nr_rect")
    {
        nw.setSolver(std::make_unique<PowerFlowNrRectSolver>());
    }
    /*
    else if (solver == "nr_pol")
    {
        nw.setSolver(std::make_unique<PowerFlowNrPolSolver>());
    }
    else if (solver == "fd")
    {
        nw.setSolver(std::make_unique<PowerFlowFdSolver>());
    }
    else if (solver == "opf_pt")
    {
        nw.setSolver(std::make_unique<PowerFlowOpfPtSolver>());
    }
    */

    map<string, int> busMap;

    nw.setUseFlatStart(!warmStart);

    Stopwatch sw;
    sw.start();
    bool success = nw.solvePowerFlow();
    sw.stop();
    cout << "(success, time) = " << success << " " << sw.seconds() << endl;

    cout << nw << std::endl;

    for (auto bus : nw.buses())
    {
        cout << 
            fixed << setw(12) << abs(bus->V()(0)) << " " << 
            fixed << setw(12) << abs(bus->V()(1)) << " " << 
            fixed << setw(12) << abs(bus->V()(2)) << " " <<
            fixed << setw(12) << abs(bus->V()(0) - bus->V()(1)) << " " << 
            fixed << setw(12) << abs(bus->V()(1) - bus->V()(2)) << " " << 
            fixed << setw(12) << abs(bus->V()(2) - bus->V()(0)) << endl;
    }
}
