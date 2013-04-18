#ifndef NODE_DOT_H
#define NODE_DOT_H

#include <vector>

namespace SmartGridToolbox
{
   class Edge;

   class Node
   {
      friend void connect(Node * node1, Node * node2);

      public:
         typedef std::vector<Edge *>::iterator EdgeIterator;
         typedef std::vector<Edge *>::const_iterator EdgeConstIterator;

         Node()
         {
            // Empty.
         }

         ~Node();

         const std::vector<Edge *> & getEdges
         {
            return edges_;
         }
         
      private:
         int idx_;
         std::vector<Edge *> edges_;
   };
}

#endif // NODE_DOT_H
