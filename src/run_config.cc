#include <SgtCore.h>
#include <SgtSim.h>

using namespace SmartGridToolbox;

int main(int argc, const char** argv)
{
   if (argc != 2)
   {
      Log().fatal() << "Usage: " << argv[0] << " config_name" << std::endl;
   }

   const char* configName = argv[1];

   Simulation sim;
   Parser<Simulation> p;
   p.parse(configName, sim);
   sim.initialize();
   bool ok = true;

   while (ok)
   {
      LogIndent _;
      ok = sim.doTimestep();
   }
}
