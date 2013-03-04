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
         typedef std::vector<Edge *>::iterator iterator;
         typedef std::vector<Edge *>::const_iterator const_iterator;

         Node()
         {
            // Empty.
         }

         ~Node()
         {
            for (iterator it : edges_)
            {
               delete *it;
            }
         }

         iterator begin()
         {
            return edges_.begin();
         }
         const_iterator begin() const
         {
            return edges_.begin();
         }

         iterator end()
         {
            return edges_.end();
         }
         const_iterator end() const
         {
            return edges_.end();
         }
         
      private:
         std::vector<Edge *> edges_;
   };

   void Connect(Node & node1, Node & node2);
}

#endif // NODE_DOT_H
