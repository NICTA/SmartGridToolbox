#ifndef NODE_DOT_H
#define NODE_DOT_H

#include <vector>

namespace MGSim
{
   class Edge;

   class Node
   {
      public:
         Node()
         {
            // Empty.
         }

         virtual ~Node()
         {
            // Empty.
         }

         const std::vector<const Edge *> & EdgesIn()
         {
            return edgesIn_;
         }

         const std::vector<const Edge *> & EdgesOut()
         {
            return edgesOut_;
         }

      private:
         std::vector<const Edge *> edgesIn_;
         std::vector<const Edge *> edgesOut_;
   };
}

#endif // NODE_DOT_H
