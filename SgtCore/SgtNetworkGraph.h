#ifndef SGT_NETWORK_GRAPH_DOT_H
#define SGT_NETWORK_GRAPH_DOT_H

#include <SgtCore/Network.h>
#include <SgtCore/NetworkGraph.h>

namespace SmartGridToolbox
{
   struct SgtGraphNodeInfo
   {
      double x;
      double y;
      double w;
      double h;
      bool locked;
      int idx;

      Bus* bus;
   };
   
   struct SgtGraphArcInfo
   {
      double l;

      double scaledX; // Useful for scaling line lengths.
      bool ignore;
      BranchAbc* branch;
   };

   using SgtGraphNode = GraphNode<SgtGraphNodeInfo, SgtGraphArcInfo>;
   using SgtGraphArc = GraphArc<SgtGraphNodeInfo, SgtGraphArcInfo>;

   class SgtNetworkGraph : public NetworkGraph<SgtGraphNodeInfo, SgtGraphArcInfo>
   {
      public:
         void create(const Network& nw);
   };
}

#endif // SGT_NETWORK_GRAPH_DOT_H
