#include "SgtNetworkGraph.h"

namespace SmartGridToolbox
{
   SgtNetworkGraph::SgtNetworkGraph(const Network& nw)
   {
      for (auto nd : nw.nodes())
      {
         auto bus = nd->bus();
         this->addNode(bus->id(),
               std::unique_ptr<SgtGraphNodeInfo>(new SgtGraphNodeInfo{0.0, 0.0, 0.0, 0.0, nd.get()}));
      }
      for (auto arc : nw.arcs())
      {
         auto branch = arc->branch();
         this->addArc(branch->id(), arc->node0()->bus()->id(), arc->node1()->bus()->id(),
               std::unique_ptr<SgtGraphArcInfo>(new SgtGraphArcInfo{0.0, arc.get()}));
      }
   };
};
