#include "PVDemoController.h"
#include "PVDemoInverter.h"
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
using namespace PVDemo;

int main(int argc, const char ** argv)
{
   if (argc != 5)
   {
      error() << "Usage: " << argv[0] << " config_name output_name voltage_lower_bound voltage_upper_bound" 
              << std::endl;
      SmartGridToolbox::abort();
   }
   
   const char * configName = argv[1];
   const char * outputName = argv[2];
   double vlb = atof(argv[3]);
   double vub = atof(argv[4]);

   std::cout << "Configuration filename = " << configName << std::endl;
   std::cout << "Output filename        = " << outputName << std::endl;
   std::cout << "Voltage lower bound    = " << vlb << std::endl;
   std::cout << "Voltage upper bound    = " << vub << std::endl;

   std::ofstream outFile(outputName);

   Model mod;
   Simulation sim(mod);
   Parser & p = Parser::globalParser();
   p.parse(configName, mod, sim);

   auto top = p.top();
   std::vector<std::string> special = top["special_inverters"].as<std::vector<std::string>>();

   std::cout << "special inverters " << std::endl;
   for (std::string name : special)
   {
      std::cout << "\t" << name << std::endl;
      SimpleInverter * inv = mod.componentNamed<SimpleInverter>(name);
      assert(inv);
      mod.replaceComponentWithNew<PVDemoInverter>(*inv);
   }
   
   Network * netw = mod.componentNamed<Network>("cdf");
   PVDemoController & pvdc = mod.newComponent<PVDemoController>("pvdc", *netw, vlb, vub);
   
   p.postParse();

   mod.validate();
   sim.initialize();

   netw->solvePowerFlow();

   auto busses = pvdc.busses();
   while (sim.doTimestep())
   {
      outFile << (dSeconds(sim.currentTime() - sim.startTime())) << " ";
      for (auto & pvdBus : busses)
      {
         Bus * sgtBus = pvdBus->sgtBus_;
         outFile <<  sgtBus->V()(0)  << " ";
      }
      outFile << std::endl;

      outFile << (dSeconds(sim.currentTime() - sim.startTime())) << " ";
      for (auto & pvdBus : busses)
      {
         Bus * sgtBus = pvdBus->sgtBus_;
         outFile <<  pvdBus->VSol_  << " ";
      }
      outFile << std::endl;

      outFile << (dSeconds(sim.currentTime() - sim.startTime())) << " ";
      for (auto & pvdBus : busses)
      {
         Bus * sgtBus = pvdBus->sgtBus_;
         outFile <<  sgtBus->STot()(0)  << " ";
      }
      outFile << std::endl;

      outFile << (dSeconds(sim.currentTime() - sim.startTime())) << " ";
      for (auto & pvdBus : busses)
      {
         Bus * sgtBus = pvdBus->sgtBus_;
         outFile <<  pvdBus->SSol_  << " ";
      }
      outFile << std::endl;
   }

   outFile.close();
}
