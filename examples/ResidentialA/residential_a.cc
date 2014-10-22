#include "SgtCore.h"
#include "SgtSim.h"

#include <fstream>

using namespace SmartGridToolbox;

double norm(ublas::vector<Complex> v)
{
   return std::sqrt(std::accumulate(v.begin(), v.end(), 0.0,
            [](double d, const Complex& c)->double{return d + std::norm(c);}));
}

ublas::vector<Complex> symComps(ublas::vector<Complex> v)
{
   static const Complex alpha = std::polar(1.0, 2.0*pi/3.0);
   static const Complex alpha2 = alpha * alpha;
   static const Array2D<Complex, 3, 3> a = {{{1, 1, 1}, {1, alpha, alpha2}, {1, alpha2, alpha}}};

   ublas::vector<Complex> result(3, czero);
   for (int i = 0; i < 3; ++i)
   {
      for (int j = 0; j < 3; ++j)
      {
         result(i) = result(i) + a[i][j] * v(j);
      }
   }
   return result;
}

int main(int argc, const char** argv)
{
   if (argc != 3)
   {
      Log().fatal() << "Usage: " << argv[0] << " config_name out_name" << std::endl;
   }

   const char* configName = argv[1];
   const char* outName = argv[2];

   std::ofstream out;
   out.open(outName);

   Simulation sim;
   Parser<Simulation> p;
   p.parse(configName, sim);
   sim.initialize();
   bool ok = true;

   std::shared_ptr<Network> netw = sim.simComponent<SimNetwork>("network")->network();

   for (auto nd : netw->nodes())
   {
      Log().message() << nd->bus()->id() << std::endl;
   }
   
   while (ok)
   {
      ok = sim.doTimestep();
      double t = dSeconds(sim.currentTime() - sim.startTime())/3600;
      out << t;
      for (auto nd : netw->nodes())
      {
         auto V = nd->bus()->V()/nd->bus()->VBase();
         for (auto x : V)
         {
            out << " " << std::abs(x);
         }
      }
      out << std::endl;
   }

   out.close();
}
