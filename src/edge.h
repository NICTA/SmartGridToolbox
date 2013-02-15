#ifndef EDGE_DOT_H
#define EDGE_DOT_H

namespace MGSim
{
   class Edge
   {
      public:
         Edge() : nodeFrom_(0),
         nodeTo_(0)
      {
         // Empty.
      }

         virtual ~Edge()
         {
            // Empty.
         }

         const Node & NodeFrom()
         {
            return *nodeFrom_;
         }

         const Node & NodeTo()
         {
            return *nodeTo_;
         }

      private:
         Node * nodeFrom_;
         Node * nodeTo_;
   };
}

#endif // EDGE_DOT_H
