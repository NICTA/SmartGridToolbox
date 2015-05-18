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

#include <iostream>

#include <SmartGridToolbox.h>

int main()
{
   using namespace SmartGridToolbox; // Import the SmartGridToolbox namespace.

   // The Model holds all components in the simulation.
   Model mod;

   // Add components to the model:
   mod.add

   // After adding all components, we must run mod.validate() to do extra processing:
   mod.validate();

   // The simulation is responsible for coordinating and advancing time for all model components:
   Simulation sim(mod);

   // Set start and end times of the simulation:
   // SmartGridToolbox uses the Time class to store both relative and absolute times.
   // Time is actually an alias for the boost::posix_time::time_duration class.
   // In the case of absolute times, it refers to the elapsed time since a constant point in the past.
   // We can create absolute times using the timeFromLocalTime and timeFromUtcTime functions, in Common.h.
   // However, if we don't care about the time of day, we can also just use relative values, as below.
   sim.setStartTime(posix_time::seconds(0)); // Relative to some arbitrary starting point. 
   sim.setEndTime(posix_time::seconds(10)); // Relative to some arbitrary starting point.

   sim.initialize(); // Initialise and advance all components in the model to the initial time.
}
