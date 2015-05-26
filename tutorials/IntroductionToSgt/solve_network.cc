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

// The following tutorial manually sets up a network and solves it using the
// Newton Raphson solver. Normally, the network would initially be parsed in
// from a YAML configuration file, but we want to show how things work behind
// the scenes. Compare to solve_network_yaml.cc.
int main(int argc, char** argv)
{
    using namespace Sgt;
    
    Network nw("my_network", 100.0); // Create a network named "my_network".
        
    auto bus1 = std::make_shared<Bus>(
            "bus_1", Phase::BAL, arma::Col<Complex>{11}, 11);
        // Create a bus named "bus_1", with a single balanced (BAL) phase,
        // a nominal voltage vector of [11 kV] (with only 1 element because it
        // is single phase), and base voltage of 11 kV. 
    bus1->setType(BusType::SL); // Make it the slack bus for powerflow.
    nw.addBus(bus1); // Add it to the network.
    
    auto gen1 = std::make_shared<GenericGen>("gen_1", Phase::BAL);
        // Create a generic generator named "gen_1".
    nw.addGen(gen1, "bus_1"); // Add it to the network, attaching it to bus_1.
   
    auto bus2 = std::make_shared<Bus>("bus_2", Phase::BAL, 
            arma::Col<Complex>{11}, 11); // Another bus...
    bus2->setType(BusType::PQ); // Make it a PQ bus (load or fixed generation).
    nw.addBus(bus2); // Add it to the network.
    
    auto load2 = std::make_shared<GenericZip>("zip_2", Phase::BAL);
        // Create a generic "ZIP" load named zip_2.
    load2->setSConst({std::polar(0.1, -5.0 * pi / 180.0)}); 
        // The load consumes a constant S component of 0.1 MW = 100 kW, at a
        // phase angle of -5 degrees.
    nw.addZip(load2, "bus_2"); // Add it to the network and to bus_2.
    
    auto line = std::make_shared<CommonBranch>("line");
        // Make a "CommonBranch" (Matpower style) line.
    line->setYSeries({Complex(0.005, -0.05)});
        // Set the series admittance of the line.
    nw.addBranch(line, "bus_1", "bus_2");
        // Add the line to the network, between the two busses.
    
    nw.solvePowerFlow();
        // Solve the power flow problem, using default Newton-Raphson solver.
    
    // Below, we show how to retrieve some information:
    auto bus = nw.bus("bus_2");
    Log().message() << "Bus " << bus->id() << ": " << " voltage is "
                    << bus->V() << std::endl; // Note logging...
}
