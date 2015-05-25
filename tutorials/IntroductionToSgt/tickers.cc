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

#include <SmartGridToolbox.h>

int main()
{
   using SmartGridToolbox::Model;
   using SmartGridToolbox::RegularUpdateComponent;
   using seconds = SmartGridToolbox::posix_time::seconds;
   using SmartGridToolbox::Simulation;

   // Create the model:
   Model mod;

   // Now populate the model using factory functionality of Model.
   // The RegularUpdateComponent does nothing other than regularly update itself, every 1 second in this case.
   RegularUpdateComponent & ticker1 = 
      mod.newComponent<RegularUpdateComponent>("ticker_1");
   ticker1.setDt(seconds(1));
   // And here's one that updates every 2 seconds...
   RegularUpdateComponent & ticker2 = 
      mod.newComponent<RegularUpdateComponent>("ticker_2");
   ticker2.setDt(seconds(2));

   mod.validate();

   // Set up the simulation.
   Simulation sim(mod);
   sim.setStartTime(seconds(0));
   sim.setEndTime(seconds(5));
   sim.initialize();

   std::cout << "Initial timestep : simulation is at time = " << sim.currentTime() << "." << std::endl;
   std::cout << "                 : ticker_1 is at time = " << ticker1.time() << "." << std::endl;
   std::cout << "                 : ticker_2 is at time = " << ticker2.time() << "." << std::endl;
   std::cout << std::endl;
   while (sim.doTimestep())
   {
      std::cout << "New timestep     : simulation is at time = " << sim.currentTime() << "." << std::endl;
      std::cout << "                 : ticker_1 is at time = " << ticker1.time() << "." << std::endl;
      std::cout << "                 : ticker_2 is at time = " << ticker2.time() << "." << std::endl;
      std::cout << std::endl;
   }
}
