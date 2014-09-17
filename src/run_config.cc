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
   std::shared_ptr<Node> bus1Nd = netw->node("bus_1");
   std::shared_ptr<Node> bus2Nd = netw->node("bus_2");
   std::shared_ptr<Node> bus3Nd = netw->node("bus_3");
   std::shared_ptr<BranchInterface> branch23 = netw->arc("line_2_3")->branch();
   
   Log().message() << "BRANCH_Y " << branch23->Y() << std::endl;

   while (ok)
   {
      Indent _;
      ok = sim.doTimestep();
      //Log().message() << "OUTPUT_1 " << norm(bus1Nd->SGen()) << " " << norm(bus1Nd->SZip()) << " " 
      //   << norm(bus1Nd->bus()->V()) << std::endl;
      //Log().message() << "OUTPUT_2 " << norm(bus2Nd->SGen()) << " " << norm(bus2Nd->SZip()) << " "
      //   << norm(bus2Nd->bus()->V()) << std::endl;
      //Log().message() << "OUTPUT_3 " << norm(bus3Nd->SGen()) << " " << norm(bus3Nd->SZip()) << " "
      //   << norm(bus3Nd->bus()->V()) << std::endl;
      Log().message() << "OUTPUT_1 " << bus1Nd->SGen() << " " << bus1Nd->SZip() << " " << norm(bus1Nd->bus()->V()) << std::endl;
      Log().message() << "OUTPUT_2 " << bus2Nd->SGen() << " " << bus2Nd->SZip() << " " << norm(bus2Nd->bus()->V()) << std::endl;
      Log().message() << "OUTPUT_3 " << bus3Nd->SGen() << " " << bus3Nd->SZip() << " " << norm(bus3Nd->bus()->V()) << std::endl;
   }
}
