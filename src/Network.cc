#include "Network.h"
#include "Edge.h"

namespace SmartGridToolbox {
   Edge & Network::MakeEdge(Node & node1, Node & node2)
   {
      Edge * edge = new Edge();
      edge->node1_ = &node1;
      edge->node2_ = &node2;
      edges_.push_back(edge);
      return *edge;
   }
}
