#ifndef NETWORK_DOT_H
#define NETWORK_DOT_H

#include "SmartGridToolbox.h"
#include <vector>

namespace SmartGridToolbox
{
   class Node;
   class Edge;

   class Network
   {
      public:
         typedef std::vector<Node>::iterator node_iterator;
         typedef std::vector<Node>::const_iterator node_const_iterator;
         typedef std::vector<Edge>::iterator edge_iterator;
         typedef std::vector<Edge>::const_iterator edge_const_iterator;

         Network()
         {
            // Empty.
         }

         ~Network()
         {
         }

         Node & MakeNode(Node & node1, Node & node2);

         Edge & MakeEdge(Node & node1, Node & node2);

      private:
         std::vector<Node *> nodes_;
         std::vector<Edge *> edges_;
   };
}

#endif // NETWORK_DOT_H
