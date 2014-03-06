#include <fstream>
#include "MicrogridController.h"
#include <SmartGridToolbox/Common.h>
#include <SmartGridToolbox/Model.h>
#include <SmartGridToolbox/Network.h>
#include <SmartGridToolbox/Parser.h>
#include <SmartGridToolbox/SimpleBattery.h>
#include <SmartGridToolbox/SimpleBuilding.h>
#include <SmartGridToolbox/SimpleInverter.h>
#include <SmartGridToolbox/Simulation.h>
#include <SmartGridToolbox/TimeSeries.h>

using namespace SmartGridToolbox;
using namespace MicrogridDemo;

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
   
   std::string configName(argv[1]);
   std::string outputName(argv[2]);

   std::cout << "Configuration filename = " << configName << std::endl;
   std::cout << "Output filename        = " << outputName << std::endl;

   std::ofstream VFile(outputName + ".out_V");
   std::ofstream SFile(outputName + ".out_S");
   std::ofstream TFile(outputName + ".out_T");

   Model mod;
   Simulation sim(mod);
   Parser & p = Parser::globalParser();
   p.registerParserPlugin<MicrogridControllerParser>();
   p.parse(configName, mod, sim);

   Network * network = mod.component<Network>("network");
   SimpleBuilding * build = mod.component<SimpleBuilding>("build");
   SimpleBattery * batt = mod.component<SimpleBattery>("batt");
   SimpleInverter * inv = mod.component<SimpleInverter>("inverter_batt");

   p.postParse();

   mod.validate();
   sim.initialize();

   auto busses = network->busVec();
   while (sim.doTimestep())
   {
      VFile << (dSeconds(sim.currentTime() - sim.startTime())) << " ";
      for (auto & bus : busses)
      {
         VFile <<  bus->V()(0)  << " ";
      }
      VFile << std::endl;

      SFile << (dSeconds(sim.currentTime() - sim.startTime())) << " ";
      for (auto & bus : busses)
      {
         SFile <<  bus->STot()(0)  << " ";
      }
      SFile << std::endl;
      
      TFile << (dSeconds(sim.currentTime() - sim.startTime())) << " " << build->Te() 
            << " " << build->Tb() << std::endl;
   }

   VFile.close();
   SFile.close();
   TFile.close();
}
