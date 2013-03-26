#include "Node.h"
#include "Edge.h"

namespace MGSim
{
   Node::~Node()
   {
      for (auto edge : edges_)
      {
         delete edge;
      }
   }
}
