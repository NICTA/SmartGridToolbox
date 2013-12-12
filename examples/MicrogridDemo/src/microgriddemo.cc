#include <fstream>
#include <SmartGridToolbox/Common.h>
#include <SmartGridToolbox/Model.h>
#include <SmartGridToolbox/Network.h>
#include <SmartGridToolbox/Parser.h>
#include <SmartGridToolbox/Simulation.h>
extern "C" {
#include <gurobi_c.h>
}

using namespace SmartGridToolbox;

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
   p.postParse();

   mod.validate();
   sim.initialize();
   Network * network = mod.componentNamed<Network>("network");
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
