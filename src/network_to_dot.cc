#include <SgtCore.h>
#include <SgtSim.h>

#include <iostream>
#include <fstream>

using namespace SmartGridToolbox;

int main(int argc, const char** argv)
{
   if (argc < 3)
   {
      Log().fatal() << "Usage: " << argv[0] << " config_name output_name [option ...]" << std::endl;
   }

   const char* configName = argv[1];
   const char* outName = argv[2];
   int nOpt = argc - 3;
   const char** opts = argv + 3;

   std::ofstream out(outName);

   Simulation sim;
   Parser<Simulation> p;
   p.parse(configName, sim);
   sim.initialize();

   std::shared_ptr<Network> netw = sim.simComponent<SimNetwork>("network")->network();

   out << "graph G {" << std::endl;
   {
      StreamIndent _(out);
      for (int i = 0; i < nOpt; ++i)
      {
         out << opts[i] << std::endl;
      }
      for (auto& nd : netw->nodes())
      {
         out << nd->bus()->id() << std::endl;
      }
      for (auto& arc : netw->arcs())
      {
         out << arc->node0()->bus()->id() << " -- " << arc->node1()->bus()->id() 
            << " [label=" << arc->branch()->id() << "]" << std::endl;
      }
   }
   out << "}" << std::endl;
   out.close();
}
