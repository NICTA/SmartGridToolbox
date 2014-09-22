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
   std::shared_ptr<Node> busANd = netw->node("bus_0");
   std::shared_ptr<Node> busBNd = netw->node("bus_10");
   std::shared_ptr<BranchInterface> branch01 = netw->arc("line_0_1")->branch();

   Log().message() << "Y = " << branch01->Y() << std::endl;
   
   while (ok)
   {
      Indent _;
      ok = sim.doTimestep();
      Log().message() << "OUTPUT_BUS_A : " << norm(busANd->SGen()) << " " << norm(busANd->SZip()) << " " <<
         norm(busANd->bus()->V()) << std::endl;
      Log().message() << "OUTPUT_BUS_B : " << norm(busBNd->SGen()) << " " << norm(busBNd->SZip()) << " " <<
         norm(busBNd->bus()->V()) << std::endl;
   }
}
