#include <SgtCore.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>

int main(int argc, char** argv)
{
   using namespace SmartGridToolbox;

   assert(argc == 3);
   std::string inFName = argv[1];
   std::string outPrefix = argv[2];
   Network nw("network", 100.0);

   std::string yamlStr =  std::string("--- [{matpower : {input_file : ") + inFName + ", default_kV_base : 11}}]";
   YAML::Node n = YAML::Load(yamlStr);

   SmartGridToolbox::Parser<Network> p;
   p.parse(n, nw);
   auto outFBus = std::fopen((outPrefix + ".bus").c_str(), "w+");
   auto outFBranch = std::fopen((outPrefix + ".branch").c_str(), "w+");
   std::map<std::string, int> busMap;
   auto print = [&](){
      printf("--------------------------------------------------------------------------\n");
      printf("%18s : %15s %15s %15s %15s %15s %15s %15s %15s %15s %15s %15s\n",
            "bus", "V_base", "|V|", "theta", "P_gen", "Q_gen", "P_load", "Q_load",
            "y_shunt_r", "y_shunt_i", "I_zip_r", "I_zip_i");
      printf("--------------------------------------------------------------------------\n");
      int iBus = 1;
      for (auto bus : nw.busses())
      {
         busMap[bus->id()] = iBus;
         auto V = nw.V2Pu(bus->V()[0], bus->VBase());
         auto SGen = bus->SGen()[0];
         auto SLoad = -bus->SZip()[0];
         auto yZip = bus->YZip()[0];
         auto IZip = bus->IZip()[0];
         printf("%18s : %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f\n",
               bus->id().c_str(), bus->VBase(), std::abs(V), std::arg(V)*180.0/pi,
               SGen.real(), SGen.imag(), SLoad.real(), SLoad.imag(),
               yZip.real(), yZip.imag(), IZip.real(), IZip.imag());
         fprintf(outFBus, "%15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f\n",
               bus->VBase(), std::abs(V), std::arg(V)*180.0/pi,
               SGen.real(), SGen.imag(), SLoad.real(), SLoad.imag(),
               yZip.real(), yZip.imag(), IZip.real(), IZip.imag());
         ++iBus;
      }
      for (auto branch : nw.branches())
      {
         auto bus0 = branch->bus0();
         auto bus1 = branch->bus1();
         int iBus0 = busMap[bus0->id()];
         int iBus1 = busMap[bus1->id()];
         fprintf(outFBranch, "%5d %5d %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f %15.10f\n", iBus0, iBus1, branch->Y()(0, 0).real(), branch->Y()(0, 0).imag(), branch->Y()(0, 1).real(), branch->Y()(0, 1).imag(), branch->Y()(1, 0).real(), branch->Y()(1, 0).imag(), branch->Y()(1, 1).real(), branch->Y()(1, 1).imag());
      }
   };
   // print();
   nw.solvePowerFlow();
   print();
   fclose(outFBus);
   fclose(outFBranch);
}
