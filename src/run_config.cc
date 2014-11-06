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

   auto bus = sim.simComponent<SimBus>("bus_11k_down");
   ublas::vector<Complex> v = bus->V();
   Bus b("bus", Phase::BAL, v, 1.0);
   const ublas::vector<Complex>& testa = b.properties().get<const ublas::vector<Complex>&>("V");
   std::cout << "test = " << testa << std::endl;
   const ublas::vector<Complex>& testb = bus->properties().get<const ublas::vector<Complex>&>("V");
   std::cout << "test = " << testb << std::endl;
}
