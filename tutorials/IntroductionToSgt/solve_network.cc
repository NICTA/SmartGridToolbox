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

#include <memory>

int main(int argc, char** argv)
{
    using namespace Sgt;
    
    // Create a network named "my_network", using a base power of 100 MW.
    Network nw("my_network", 100.0);
        
    auto bus1 = std::make_shared<Bus>("bus_1", Phase::BAL, {11}, 11);
        // Create a bus named "bus_1", with a single balanced (BAL) phase,
        // a nominal voltage vector of [11 kV] (with only 1 element because it
        // is single phase), and base voltage of 11 kV. 
    bus1->setType(BusType::SL);
        // Make it the slack bus for powerflow.
    nw.addBus(bus1);
        // Add it to the network.
    
    auto gen1 = std::make_shared<GenericGen>("gen_1", Phase::BAL);
        // Create a generic generator named "gen_1".
    nw.addGen(gen1, "bus_1");
        // Add it to the network, attaching it to bus_1.
   
    // Create another bus...
    auto bus2 = std::make_shared<Bus>("bus_2", Phase::BAL, {11}, 11);
    bus2->setType(BusType::PQ);
        // Make it a PQ bus (load or fixed generation).

}
