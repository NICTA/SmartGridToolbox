#include <SmartGridToolbox/Parser.h>
#include <SmartGridToolbox/Simulation.h>

using namespace SmartGridToolbox;

int main(int argc, const char** argv)
{
   if (argc != 2)
   {
      Log().fatal() << "Usage: " << argv[0] << " config_name" << std::endl;
   }

   const char* configName = argv[1];

   Simulation sim;
   SmartGridToolbox::Parser<Simulation> p;
   p.parse(configName, sim);
   sim.initialize();
   bool ok = true;
   while (ok)
   {
      Log().message() << "Doing timestep at " << sim.currentTime() << std::endl;
      Indent _;
      ok = sim.doTimestep();
      Log().message() << "Now at " << sim.currentTime() << std::endl;
   }
}
