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
   while(sim.doTimestep())
   {
      std::cout << "New timestep     : simulation is at time = " << sim.currentTime() << "." << std::endl;
      std::cout << "                 : ticker_1 is at time = " << ticker1.time() << "." << std::endl;
      std::cout << "                 : ticker_2 is at time = " << ticker2.time() << "." << std::endl;
      std::cout << std::endl;
   }
}
