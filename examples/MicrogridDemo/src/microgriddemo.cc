#include <fstream>
#include <SmartGridToolbox/Common.h>
#include <SmartGridToolbox/Model.h>
#include <SmartGridToolbox/Network.h>
#include <SmartGridToolbox/Parser.h>
#include <SmartGridToolbox/SimpleBuilding.h>
#include <SmartGridToolbox/Simulation.h>
extern "C" {
#include <gurobi_c.h>
}

using namespace SmartGridToolbox;

static double sinusoidal(double t, double T, double Delta, double minim, double maxim)
{
   // Sinusoidal function, T is period, Delta is offset.
   return minim + (maxim - minim) * 0.5 * (1.0 + cos(2.0 * pi * (t - Delta) / T));
}

int main(int argc, const char ** argv)
{
   if (argc != 3)
   {
      error() << "Usage: " << argv[0] << " config_name output_name" << std::endl;
      SmartGridToolbox::abort();
   }
   
   const char * configName = argv[1];
   const char * outputName = argv[2];

   std::cout << "Configuration filename = " << configName << std::endl;
   std::cout << "Output filename        = " << outputName << std::endl;

   std::ofstream outFile(outputName);

   Model mod;
   Simulation sim(mod);
   Parser & p = Parser::globalParser();
   p.parse(configName, mod, sim);

   Network * network = mod.componentNamed<Network>("network");
   SimpleBuilding * build = mod.componentNamed<SimpleBuilding>("build");
   Time t0 = posix_time::seconds(0);
   auto Te = [&](Time t){return sinusoidal(dSeconds(t-t0), day, 12*hour, 10*K, 28*K);};
   auto dQg = [&](Time t){return sinusoidal(dSeconds(t-t0), day, 14*hour, 40*kW, 60*kW);};
   build->setTeFunc(Te);
   build->set_dQgFunc(dQg);

   p.postParse();

   mod.validate();
   sim.initialize();
   auto busses = network->busVec();
   while (sim.doTimestep())
   {
      outFile << (dSeconds(sim.currentTime() - sim.startTime())) << " ";
      for (auto & bus : busses)
      {
         outFile <<  bus->V()(0)  << " ";
      }
      outFile << std::endl;

      outFile << (dSeconds(sim.currentTime() - sim.startTime())) << " ";
      for (auto & bus : busses)
      {
         outFile <<  bus->STot()(0)  << " ";
      }
      outFile << std::endl;
   }

   outFile.close();
}
