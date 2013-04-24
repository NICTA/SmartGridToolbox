#include "Node.h"
#include "Edge.h"

namespace SmartGridToolbox
{
   Node::~Node()
   {
      for (auto edge : edges_)
      {
         delete edge;
      }
   }
}
