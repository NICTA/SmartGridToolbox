#include "SgtCore.h"
#include "SgtSim.h"

#include <fstream>

using namespace SmartGridToolbox;

double norm(arma::Col<Complex> v)
{
   return std::sqrt(std::accumulate(v.begin(), v.end(), 0.0,
            [](double d, const Complex& c)->double{return d + std::norm(c);}));
}

arma::Col<Complex> symComps(arma::Col<Complex> v)
{
   static const Complex alpha = std::polar(1.0, 2.0*pi/3.0);
   static const Complex alpha2 = alpha * alpha;
   static const Array2D<Complex, 3, 3> a = {{{1, 1, 1}, {1, alpha, alpha2}, {1, alpha2, alpha}}};

   arma::Col<Complex> result(3, arma::fill::zeros);
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

   out << "# t";
   for (auto nd : netw->nodes())
   {
      const Phases& ps = nd->bus()->phases();
      for (int i = 0; i < ps.size(); ++i)
      {
         Phase p = ps[i];
         out << " " << nd->bus()->id() << ":" << p;
      }
   }
   out << std::endl;
  
   out << 0;
   for (auto nd : netw->nodes())
   {
      const Phases& ps = nd->bus()->phases();
      for (int i = 0; i < ps.size(); ++i)
      {
         Phase p = ps[i];
         int iPhase = 0;
         if (p == Phase::BAL)
         {
            iPhase = 0;
         }
         if (p == Phase::A)
         {
            iPhase = 1;
         }
         else if (p == Phase::B)
         {
            iPhase = 2;
         }
         else if (p == Phase::C)
         {
            iPhase = 3;
         }
         out << " " << iPhase;
      }
   }
   out << std::endl;
   
   while (ok)
   {
      ok = sim.doTimestep();
      double t = dSeconds(sim.currentTime() - sim.startTime())/3600;
      out << t;
      for (auto nd : netw->nodes())
      {
         arma::Col<Complex> V = nd->bus()->V()/nd->bus()->VBase();
         for (auto x : V)
         {
            out << " " << std::abs(x);
         }
      }
      out << std::endl;
   }

   out.close();
}
