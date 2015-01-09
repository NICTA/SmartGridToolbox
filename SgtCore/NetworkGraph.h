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
   struct BasicGraphNodeInfo
   {
      double x{0.0};
      double y{0.0};
      double w{0.0};
      double h{0.0};
   };
   
   struct BasicGraphArcInfo
   {
      double l{0.0};
   };

   template<typename NI = BasicGraphNodeInfo, typename AI = BasicGraphArcInfo> struct GraphTraits
   {
      typedef NI GraphNodeInfo; 
      typedef AI GraphArcInfo; 
   };

   using BasicGraphTraits = GraphTraits<BasicGraphNodeInfo, BasicGraphArcInfo>; 

   template<typename GT> struct GraphArc;

   template<typename GT = BasicGraphTraits> struct GraphNode
   {
      std::string id;
      std::vector<GraphArc<GT>*> adjacentArcs;
      std::unique_ptr<typename GT::GraphNodeInfo> info;

      GraphNode(const std::string& id, std::unique_ptr<typename GT::GraphNodeInfo> info) : 
         id(id), info(std::move(info))
      {
         // Empty.
      }
   };

   template<typename GT = BasicGraphTraits> struct GraphArc
   {
      std::string id;
      const GraphNode<GT>* n0;
      const GraphNode<GT>* n1;
      std::unique_ptr<typename GT::GraphArcInfo> info;
      
      GraphArc(const std::string& id, const GraphNode<GT>* n0, const GraphNode<GT>* n1,
            std::unique_ptr<typename GT::GraphArcInfo> info) :
         id(id), n0(n0), n1(n1), info(std::move(info)) 
      {
         // Empty.
      }
   };
   
   void layoutOgdf(ogdf::Graph& g, ogdf::GraphAttributes ga, int layoutType);

   template<typename GT = BasicGraphTraits> class NetworkGraph
   {
      public:

         ~NetworkGraph();

         void addNode(const std::string& id, std::unique_ptr<typename GT::GraphNodeInfo> info)
         {
            nodeMap_.emplace(std::make_pair(id, GraphNode<GT>{id, std::move(info)}));
         }

         void addArc(const std::string& id, const std::string& id0, const std::string& id1,
               std::unique_ptr<typename GT::GraphArcInfo> info)
         {
            auto& n0 = nodeMap_.at(id0);
            auto& n1 = nodeMap_.at(id1);
            auto a = GraphArc<GT>(id, &n0, &n1, std::move(info));
            n0.adjacentArcs.push_back(&a);
            arcMap_.emplace(std::make_pair(id, std::move(a)));
         }

         void layout()
         {
            ogdf::Graph g;
            ogdf::GraphAttributes ga(g, ogdf::GraphAttributes::nodeGraphics | ogdf::GraphAttributes::edgeGraphics);

            std::map<std::string, ogdf::node> ogdfNdMap;
            std::vector<GraphNode<GT>*> nodeVec(nodeMap_.size());

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

         const std::map<std::string, GraphNode<GT>>& nodes() const
         {
            return nodeMap_;
         }

         const std::map<std::string, GraphArc<GT>>& arcs() const
         {
            return arcMap_;
         }

      private:

         void validate();

      private:

         std::map<std::string, GraphNode<GT>> nodeMap_;
         std::map<std::string, GraphArc<GT>> arcMap_;
   };

} // namespace SmartGridToolbox

#endif // NETWORKGRAPH_H
