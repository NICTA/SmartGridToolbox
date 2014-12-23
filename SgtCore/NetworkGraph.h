#ifndef NETWORKGRAPH_H
#define NETWORKGRAPH_H

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace SmartGridToolbox
{
   struct NetworkNodeInfo
   {
      double x{0.0};
      double y{0.0};
      double w{0.0};
      double h{0.0};
   };
   
   struct NetworkArcInfo
   {
      double l{0.0};
   };
   
   struct NetworkArc;

   struct NetworkNode
   {
      std::string id;
      std::vector<NetworkArc*> adjacentArcs;
      std::unique_ptr<NetworkNodeInfo> info;

      NetworkNode(const std::string& id, std::unique_ptr<NetworkNodeInfo> info) : id(id), info(std::move(info)) {}
   };

   struct NetworkArc
   {
      std::string id;
      const NetworkNode* n0;
      const NetworkNode* n1;
      std::unique_ptr<NetworkArcInfo> info;
      
      NetworkArc(const std::string& id, const NetworkNode* n0, const NetworkNode* n1,
            std::unique_ptr<NetworkArcInfo> info) :
         id(id), n0(n0), n1(n1), info(std::move(info)) {}
   };

   class NetworkGraph
   {
      public:

         ~NetworkGraph();

         void addNode(const std::string& id, std::unique_ptr<NetworkNodeInfo> info)
         {
            nodeMap_.emplace(std::piecewise_construct, std::forward_as_tuple(id)
                  
                  std::make_pair(id, NetworkNode{id, std::move(info)}));
         }

         void addArc(const std::string& id, const std::string& id0, const std::string& id1,
               std::unique_ptr<NetworkArc> info);

         void layout();

         const std::map<std::string, NetworkNode>& nodes() const
         {
            return nodeMap_;
         }

         const std::map<std::string, NetworkArc>& arcs() const
         {
            return arcMap_;
         }

      private:

         void validate();

      private:

         std::map<std::string, NetworkNode> nodeMap_;
         std::map<std::string, NetworkArc> arcMap_;
   };

} // namespace SmartGridToolbox

#endif // NETWORKGRAPH_H
