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

using namespace Sgt;

int main(int argc, const char** argv)
{
    if (argc != 2)
    {
        sgtError("Usage: " << argv[0] << " config_name.");
    }

    const char* configName = argv[1];

    Simulation sim;
    Parser<Simulation> p;
    p.parse(configName, sim);
    sim.initialize();

    sim.simComponent<SimNetwork>("network")->network()->print(std::cout);

    while (!sim.isFinished())
    {
        LogIndent _;
        sim.doTimestep();
    }
}
