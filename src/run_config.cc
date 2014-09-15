#include "LibSgtCore.h"
#include "LibSgtSim.h"

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

   auto spv = sim.simComponent<SolarPv>("solar_pv");
   auto inv = sim.simComponent<SimInverter>("inverter")->zip();
   auto bus = sim.simComponent<SimBus<Bus>>("bus_2")->bus();

   while (ok)
   {
      Log().message() << "Doing timestep at " << sim.currentTime() << std::endl;
      Indent _;
      ok = sim.doTimestep();
      Log().message() << "Now at " << sim.currentTime() << std::endl;
      Log().message() << std::norm(bus->V()[0]) << " " << spv->PDc() << " " << inv->PDc() << " " << inv->SConst() << std::endl;
   }
}
