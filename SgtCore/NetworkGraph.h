#ifndef NETWORKGRAPH_H
#define NETWORKGRAPH_H

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace SmartGridToolbox
{
   struct NodeInfo
   {
      double x{0.0};
      double y{0.0};
      double w{0.0};
      double h{0.0};
   };
   
   struct ArcInfo
   {
      double l{0.0};
   };

   template<typename NI = NodeInfo, typename AI = ArcInfo> struct GraphTraits
   {
      typedef NI NodeInfoType; 
      typedef AI ArcInfoType; 
   };

   using BasicGraphTraits = GraphTraits<NodeInfo, ArcInfo>; 

   template<typename GT> struct NetworkArc;

   template<typename GT = BasicGraphTraits> struct NetworkNode
   {
      std::string id;
      std::vector<NetworkArc<GT>*> adjacentArcs;
      std::unique_ptr<typename GT::NodeInfoType> info;

      NetworkNode(const std::string& id, std::unique_ptr<typename GT::NodeInfoType> info) : 
         id(id), info(std::move(info))
      {
         // Empty.
      }
   };

   template<typename GT = BasicGraphTraits> struct NetworkArc
   {
      std::string id;
      const NetworkNode<GT>* n0;
      const NetworkNode<GT>* n1;
      std::unique_ptr<typename GT::ArcInfoType> info;
      
      NetworkArc(const std::string& id, const NetworkNode<GT>* n0, const NetworkNode<GT>* n1,
            std::unique_ptr<typename GT::NetworkArcInfoType> info) :
         id(id), n0(n0), n1(n1), info(std::move(info)) 
      {
         // Empty.
      }
   };
   
   void layoutOgdf(ogdf::Graph& g, ogdf::GraphAttributes ga, int layoutType);

   template<typename GT> class NetworkGraph
   {
      public:

         ~NetworkGraph();

         void addNode(const std::string& id, std::unique_ptr<typename GT::NodeInfoType> info)
         {
            nodeMap_.emplace(std::make_pair(id, NetworkNode<GT>{id, std::move(info)}));
         }

         void addArc(const std::string& id, const std::string& id0, const std::string& id1,
               std::unique_ptr<typename GT::ArcInfoType> info)
         {
            auto& n0 = nodeMap_.at(id0);
            auto& n1 = nodeMap_.at(id1);
            auto a = NetworkArc<GT>(id, &n0, &n1, std::move(info));
            n0.adjacentArcs.push_back(&a);
            arcMap_.emplace(std::make_pair(id, std::move(a)));
         }

         void layout()
         {
            ogdf::Graph g;
            ogdf::GraphAttributes ga(g, ogdf::GraphAttributes::nodeGraphics | ogdf::GraphAttributes::edgeGraphics);

            std::map<std::string, ogdf::node> ogdfNdMap;
            std::vector<NetworkNode<GT>*> nodeVec(nodeMap_.size());

            for (auto& n : nodeMap_)
            {
               ogdf::node ogdfNd = g.newNode();
               ogdfNdMap.at(n.second.id) = ogdfNd;
               nodeVec[ogdfNd->index()] = &n.second;
            }

            for (auto& a : arcMap_)
            {
               const std::string& id0 = a.second.n0->id;
               const std::string& id1 = a.second.n1->id;
               g.newEdge(ogdfNdMap[id0], ogdfNdMap[id1]);
            }

            ogdf::node n;
            forall_nodes(n, g)
            {
               ga.width(n) = nodeVec[n->index()]->info->w;
               ga.height(n) = nodeVec[n->index()]->info->h;
            }

            layoutOgdf(g, ga, 0);

            forall_nodes(n, g)
            {
               int idx = n->index();
               nodeVec[idx]->info->x = ga.x(n);
               nodeVec[idx]->info->y = ga.y(n);
            }
         }

         const std::map<std::string, NetworkNode<GT>>& nodes() const
         {
            return nodeMap_;
         }

         const std::map<std::string, NetworkArc<GT>>& arcs() const
         {
            return arcMap_;
         }

      private:

         void validate();

      private:

         std::map<std::string, NetworkNode<GT>> nodeMap_;
         std::map<std::string, NetworkArc<GT>> arcMap_;
   };

} // namespace SmartGridToolbox

#endif // NETWORKGRAPH_H
