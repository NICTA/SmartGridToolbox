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

    std::string outFName = "out.txt";
    {
        const char* opt = getCmdOption(argv, argc, "--prefix");
        if (opt != nullptr) outFName = opt;
    }

    bool warmStart = cmdOptionExists(argv, argc, "--warm_start");
    
    bool debug = cmdOptionExists(argv, argc, "--debug");

    if (debug)
    {
        std::cout << "debug" << std::endl;
        Sgt::messageLogLevel() = Sgt::LogLevel::VERBOSE;
        Sgt::warningLogLevel() = Sgt::LogLevel::VERBOSE;
        Sgt::debugLogLevel() = Sgt::LogLevel::NORMAL;
    }

    Network nw(100.0);
    Parser<Network> p;
    p.parse(inFName, nw);

    std::ofstream outF(outFName.c_str());
    std::map<std::string, int> busMap;

    nw.setUseFlatStart(!warmStart);

    Stopwatch sw;
    sw.start();
    bool success = nw.solvePowerFlow();
    sw.stop();
    std::cout << "(success, time) = " << success << " " << sw.seconds() << std::endl;
    outF << nw << std::endl;
}
