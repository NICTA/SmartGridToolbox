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

   out << "digraph G {" << std::endl;
   {
      StreamIndent _(out);
      out << "layout=neato;" << std::endl;
      out << "mode=ipsep;" << std::endl;
      out << "overlap=ipsep;" << std::endl;
      out << "sep=0.5;" << std::endl;
      out << "node [shape=circle, width=1, fixedsize=true, style=filled];" << std::endl;
      out << "edge [len=2.5];" << std::endl;
      for (int i = 0; i < nOpt; ++i)
      {
         out << opts[i] << ";" << std::endl;
      }
      for (auto& nd : netw->nodes())
      {
         const auto& bus = *nd->bus();
         std::string col;
         switch (bus.type())
         {
            case BusType::SL:
               col = "deeppink";
               break;
            case BusType::PV:
               col = "orange";
               break;
            case BusType::PQ:
               col = "lightblue";
               break;
            default:
               col = "black";
               break;
         }
         out << bus.id() << " [" << "color=" << col << "];" << std::endl;
         for (auto& gen : nd->gens())
         {
            out << gen->id() << " [shape=point, width=0.1, color=orange" << "];" << std::endl;
            out << gen->id() << " -> " << bus.id() << " [len=1, color=orange];" << std::endl;
         }
         for (auto& zip : nd->zips())
         {
            out << zip->id() << " [shape=point, width=0.1, color=lightblue];" << std::endl;
            out << bus.id() << " -> " << zip->id() << " [len=1, color=lightblue];" << std::endl;
         }
      }
      for (auto& arc : netw->arcs())
      {
         // out << arc->node0()->bus()->id() << " -> " << arc->node1()->bus()->id() 
         //   << " [label=" << arc->branch()->id() << ", dir=none];" << std::endl;
         out << arc->node0()->bus()->id() << " -> " << arc->node1()->bus()->id() 
            << " [tooltip=" << arc->branch()->id() << ", dir=none];" << std::endl;
      }
   }
   out << "}" << std::endl;
   out.close();
}
