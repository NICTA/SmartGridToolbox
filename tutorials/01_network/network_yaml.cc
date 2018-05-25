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

#include <SgtCore.h> // Include SgtCore headers.

// The following tutorial sets up a network by parsing in a YAML file, and solves it using the Newton Raphson solver.
int main(int argc, char** argv)
{
    using namespace Sgt;
    
    Network nw;
        // Create a network named "network".
    Parser<Network> p; 
        // Make a network parser to parse information into the network.
    p.parse("network.yaml", nw);
        // Read in from the config file "network.yaml".
        
    nw.solvePowerFlow();
        // Solve the power flow problem, using default Newton-Raphson solver.
    
    // We show below how to retrieve some information:
    auto bus = nw.buses()["bus_2"];
    Log().message() << "Bus " << bus->id() << ": " << " voltage is " << bus->V() << std::endl;
        // Note logging...

    Log().message() << "Network: " << nw << std::endl;
        // Print the network.
}
