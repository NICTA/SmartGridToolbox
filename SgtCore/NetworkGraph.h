#ifndef NETWORKGRAPH_H
#define NETWORKGRAPH_H

#include <map>
#include <string>
#include <vector>

namespace SmartGridToolbox
{
   struct NetworkArc;

   struct NetworkNode
   {
      std::string id;
      double x{0.0};
      double y{0.0};
      double w{0.0};
      double h{0.0};
      std::vector<NetworkArc*> adjacentArcs;

      NetworkNode(const std::string& id, double x, double y, double w, double h);
   };

   struct NetworkArc
   {
      std::string id;
      double l;
      NetworkNode* n0;
      NetworkNode* n1;
   };

   class NetworkGraph
   {
      public:

         ~NetworkGraph();

         void addNode(const std::string& id, double x, double y, double w, double h);
         void addArc(const std::string& id, double l, const std::string& id0, const std::string& id1);

         void layout();

         const std::map<std::string, NetworkNode*>& nodes() const {return nodeMap_;}
         const std::map<std::string, NetworkArc*>& arcs() const {return arcMap_;}

      private:

         void validate();

      private:

         std::map<std::string, NetworkNode*> nodeMap_;
         std::map<std::string, NetworkArc*> arcMap_;
   };
} // namespace SmartGridToolbox

#endif // NETWORKGRAPH_H
