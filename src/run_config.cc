#include "LibSgtCore.h"
#include "LibSgtSim.h"

using namespace SmartGridToolbox;

double norm(ublas::vector<Complex> v)
{
   return std::sqrt(std::accumulate(v.begin(), v.end(), 0.0,
            [](double d, const Complex& c)->double{return d + std::norm(c);}));
}

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

   std::shared_ptr<Network> netw = sim.simComponent<SimNetwork>("network")->network();
   std::shared_ptr<Node> bus0Nd = netw->node("bus_0");
   std::shared_ptr<Node> bus20Nd = netw->node("bus_20");
   
   while (ok)
   {
      Indent _;
      ok = sim.doTimestep();
      Log().message() << "OUTPUT_BUS_0  : " << norm(bus0Nd->SGen()) << " " << norm(bus0Nd->SZip()) << " " 
      Log().message() << "OUTPUT_BUS_20 : " << norm(bus20Nd->SGen()) << " " << norm(bus20Nd->SZip()) << " " 
   }
}
