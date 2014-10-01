#include "LibSgtCore.h"
#include "LibSgtSim.h"

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
      Log().fatal() << "Usage: " << argv[0] << " config_name bus_id" << std::endl;
   }

   const char* configName = argv[1];
   const char* busId = argv[2];

   Simulation sim;
   Parser<Simulation> p;
   p.parse(configName, sim);
   sim.initialize();
   bool ok = true;

   std::shared_ptr<Network> netw = sim.simComponent<SimNetwork>("network")->network();
   std::shared_ptr<Node> busNd = netw->node(busId);

   while (ok)
   {
      Indent _;
      ok = sim.doTimestep();
      double t = dSeconds(sim.currentTime() - sim.startTime())/3600;
      auto V = busNd->bus()->V();
      auto S = busNd->SZip();
      {
         {
            auto& msg = Log().message() << "V_MAG : " << t;
            for (auto x : V) msg << " " << std::abs(x); 
            msg << std::endl;
         }
         {
            auto& msg = Log().message() << "V_ANG : " << t;
            for (auto x : V) msg << " " << std::arg(x); 
            msg << std::endl;
         }
         {
            auto& msg = Log().message() << "P     : " << t;
            for (auto x : S) msg << " " << std::real(x); 
            msg << std::endl;
         }
         {
            auto& msg = Log().message() << "Q     : " << t;
            for (auto x : S) msg << " " << std::imag(x); 
            msg << std::endl;
         }
      }
   }
}
