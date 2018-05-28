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

int main(int argc, char** argv)
{
    using namespace Sgt;
    
    Simulation sim;
        // The top level simulation object.
    Parser<Simulation> p;
        // Parse information into sim from config file "simulation.yaml".
    p.parse("simulation.yaml", sim);
    
        // We just parsed in a simulation. The user is also free to modify this programmatically, e.g. below we add
        // an extra zip load to the network.
    auto simNetw = sim.simComponent<SimNetwork>("network");
        // Get network by name.
    auto extraZip = std::make_shared<Zip>("extra_zip", Phase::BAL);
        // Make a Zip.
    extraZip->setYConst({1e-3});
        // Modify it: constant impedance load.
    auto extraZipCompPtr = simNetw->network().addZip(extraZip, "bus_2");
        // Add it to the network, returning a ComponentPtr to the component.
    auto simExtraZip = sim.newSimComponent<SimZip>(extraZipCompPtr); 
        // Give it to a new SimZip for the Simulation.

        // After parsing or making other modifications, we need to call initialize. All components will be initialized
        // to time sim.startTime().
    sim.initialize();

    Log().message() << *sim.simComponent<SimNetwork>("network") << std::endl;
        // Print the network, at the start of the simulation.

    auto bus2 = simNetw->network().buses()["bus_2"];
        // Get bus by name.

    while (!sim.isFinished())
    {
        Log().message() << "Timestep " << sim.currentTime() << std::endl;
        sim.doTimestep();
            // Perform a single timestep: keep updating components until (1) the time has increased and all components
            // are up to date at that time, or (2) the simulation finishes. Finer grained control is also possible,
            // e.g. sim.doNextUpdate() will update one component at a time.
        {
            sgtLogIndent();
                // Indent within scope.
            Log().message() << "bus_2 V = " << bus2->V() << std::endl;
                // Log the voltage at the current time.
        }
    }
}
