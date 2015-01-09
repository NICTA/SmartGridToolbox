#include "SgtNetworkGraph.h"

namespace SmartGridToolbox
{
   void SgtNetworkGraph::create(const Network& nw)
   {
      for (auto nd : nw.nodes())
      {
         auto bus = nd->bus();
         this->addNode(bus->id(), SgtGraphNodeInfo{0.0, 0.0, 10.0, 10.0, nd.get()});
      }
      for (auto arc : nw.arcs())
      {
         auto branch = arc->branch();
         this->addArc(branch->id(), arc->node0()->bus()->id(), arc->node1()->bus()->id(), 
               SgtGraphArcInfo{0.0, arc.get()});
      }
   };
};
