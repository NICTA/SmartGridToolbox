#include <SmartGridToolbox.h>

int main()
{
   //using namespace SmartGridToolbox;
   using SmartGridToolbox::Bus;
   using SmartGridToolbox::Model;
   using SmartGridToolbox::Parser;
   using SmartGridToolbox::Simulation;
   using SmartGridToolbox::operator<<;

   Model mod;
   Simulation sim(mod);
   Parser & p = Parser::globalParser();
   p.parse("test_overhead_line.yaml", mod, sim); p.postParse();
   mod.validate();
   sim.initialize();
   // That's it!

   // Since there is no time dependence this model, none of the components will ever undergo an update. 
   // But, the initial call to sim.initialize() will ensure that the model is put into a valid state, with its time
   // equal to the start time of the simulation in the yaml configuration file. Thus, the power flow equations will
   // already be solved. We can look at the voltages etc. on the busses:
   
   Bus * bus_1 = mod.component<Bus>("bus_1");
   Bus * bus_2 = mod.component<Bus>("bus_2");

   std::cout << "Bus 1 complex voltage = " << bus_1->V() << std::endl;
   std::cout << "Bus 2 complex voltage = " << bus_2->V() << std::endl;
}
