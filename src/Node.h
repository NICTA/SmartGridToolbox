#ifndef NODE_DOT_H
#define NODE_DOT_H

#include <vector>

namespace MGSim
{
   class Edge;

   class Node
   {
      friend void Connect(Node * node1, Node * node2);

      public:
         typedef std::vector<Edge *>::iterator EdgeIterator;
         typedef std::vector<Edge *>::const_iterator EdgeConstIterator;

         Node()
         {
            // Empty.
         }

         ~Node();

         EdgeIterator begin()
         {
            return edges_.begin();
         }
         EdgeConstIterator begin() const
         {
            return edges_.begin();
         }

         EdgeIterator end()
         {
            return edges_.end();
         }
         EdgeConstIterator end() const
         {
            return edges_.end();
         }
         
      private:
         int idx_;
         std::vector<Edge *> edges_;
   };
}

#endif // NODE_DOT_H
