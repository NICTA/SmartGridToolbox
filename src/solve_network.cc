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
   std::cout << yamlStr << std::endl;
   YAML::Node n = YAML::Load(yamlStr);

   SmartGridToolbox::Parser<Network> p;
   p.parse(n, nw);
   auto outFBus = std::fopen((outPrefix + ".bus").c_str(), "w+");
   auto outFBranch = std::fopen((outPrefix + ".branch").c_str(), "w+");
   std::map<std::string, int> busMap;
   auto print = [&](){
      printf("--------------------------------------------------------------------------\n");
      printf("%12s : %9s %9s %9s %9s %9s %9s %9s\n",
            "bus", "V_base", "|V|", "theta", "P_gen", "Q_gen", "P_load", "Q_load");
      printf("--------------------------------------------------------------------------\n");
      int iBus = 1;
      for (auto nd : nw.nodes())
      {
         auto bus = nd->bus();
         busMap[bus->id()] = iBus;
         auto V = nw.V2Pu(bus->V()[0], bus->VBase());
         auto SGen = nd->SGen()[0];
         auto SLoad = -nd->SZip()[0];
         printf("%12s : %9.4f %9.4f %9.4f %9.4f %9.4f %9.4f %9.4f\n",
               bus->id().c_str(), bus->VBase(), std::abs(V), std::arg(V)*180.0/pi,
               SGen.real(), SGen.imag(), SLoad.real(), SLoad.imag());
         fprintf(outFBus, "%9.4f %9.4f %9.4f %9.4f %9.4f %9.4f %9.4f\n",
               bus->VBase(), std::abs(V), std::arg(V)*180.0/pi,
               SGen.real(), SGen.imag(), SLoad.real(), SLoad.imag());
      }
      for (auto arc : nw.arcs())
      {
         auto branch = std::dynamic_pointer_cast<CommonBranch>(arc->branch());
         auto bus0 = arc->node0()->bus();
         auto bus1 = arc->node1()->bus();
         int iBus0 = busMap[bus0->id()];
         int iBus1 = busMap[bus1->id()];
         fprintf(outFBranch, "%5d %5d %9.4f %9.4f %9.4f %9.4f %9.4f %9.4f %9.4f %9.4f\n", iBus0, iBus1, branch->Y()(0, 0).real(), branch->Y()(0, 0).imag(), branch->Y()(0, 0).real(), branch->Y()(0, 0).imag(), branch->Y()(0, 0).real(), branch->Y()(0, 0).imag(), branch->Y()(0, 0).real(), branch->Y()(0, 0).imag());
      }
   };
   // print();
   nw.solvePowerFlow();
   print();
   fclose(outFBus);
   fclose(outFBranch);
}
