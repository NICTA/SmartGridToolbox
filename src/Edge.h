#ifndef EDGE_DOT_H
#define EDGE_DOT_H

namespace SmartGridToolbox
{
   class Edge
   {
      friend class Network;

      public:
         virtual ~Edge()
         {
            // Empty.
         }

         const Node & getNode1()
         {
            return *node1_;
         }

         const Node & getNode2()
         {
            return *node2_;
         }

      private:

         Edge() : node1_(0), node2_(0)
         {
            // Empty.
         }

      private:

         Node * node1_;
         Node * node2_;
   };
}

#endif // EDGE_DOT_H
