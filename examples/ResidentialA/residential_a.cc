#include "Appliance.h"
#include "ApplianceParserPlugin.h"

#include <SgtCore.h>
#include <SgtSim.h>

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
   p.registerParserPlugin<ApplianceParserPlugin>();
   p.parse(configName, sim);
   sim.initialize();
   bool ok = true;

   std::shared_ptr<Network> netw = sim.simComponent<SimNetwork>("network")->network();

   out << "# t";
   for (auto bus : netw->busses())
   {
      out << " " << bus->id();
   }
   out << std::endl;
  
   Stopwatch swTot;
   swTot.start();
   while (ok)
   {
      ok = sim.doTimestep();
      double t = dSeconds(sim.currentTime() - sim.startTime())/3600;
      out << t;
      for (ConstBusPtr bus : netw->busses())
      {
         auto S = bus->SZip();
         double P = 0;
         for (auto& Si : S)
         {
            P += Si.real();
         }
         out << " " << P;
         
      }
      out << std::endl;
   }
   swTot.stop();
   Log().message() << "Total time = " << swTot.seconds() << std::endl;

   out.close();
}
