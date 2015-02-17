#ifndef NETWORKGRAPH_H
#define NETWORKGRAPH_H

#include <ogdf/basic/EdgeArray.h>
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
      double x;
      double y;
      double w;
      double h;
   };
   
   struct BasicGraphArcInfo
   {
      double l;
   };

   template<typename NI = BasicGraphNodeInfo, typename AI = BasicGraphArcInfo> struct GraphTraits
   {
      using GraphNodeInfo = NI; 
      using GraphArcInfo = AI; 
   };

   using BasicGraphTraits = GraphTraits<BasicGraphNodeInfo, BasicGraphArcInfo>; 

   template<typename GT> struct GraphArc;

   template<typename GT = BasicGraphTraits> struct GraphNode
   {
      std::string id;
      std::vector<GraphArc<GT>*> adjacentArcs;
      typename GT::GraphNodeInfo info;

      GraphNode(const std::string& id, const typename GT::GraphNodeInfo& info) : 
         id(id), info(info)
      {
         // Empty.
      }
   };

   template<typename GT = BasicGraphTraits> struct GraphArc
   {
      std::string id;
      const GraphNode<GT>* n0;
      const GraphNode<GT>* n1;
      typename GT::GraphArcInfo info;
      
      GraphArc(const std::string& id, const GraphNode<GT>* n0, const GraphNode<GT>* n1,
            const typename GT::GraphArcInfo& info) :
         id(id), n0(n0), n1(n1), info(info) 
      {
         // Empty.
      }
   };
   
   void layoutOgdf(ogdf::Graph& g, ogdf::GraphAttributes& ga, ogdf::EdgeArray<double>& edgeLengths, int layoutType);

   template<typename GT = BasicGraphTraits> class NetworkGraph
   {
      public:

         void addNode(const std::string& id, const typename GT::GraphNodeInfo& info)
         {
            nodeMap_.emplace(std::make_pair(id, GraphNode<GT>{id, info}));
         }

         void addArc(const std::string& id, const std::string& id0, const std::string& id1,
               const typename GT::GraphArcInfo& info)
         {
            auto& n0 = nodeMap_.at(id0);
            auto& n1 = nodeMap_.at(id1);
            auto a = GraphArc<GT>(id, &n0, &n1, info);
            n0.adjacentArcs.push_back(&a);
            arcMap_.emplace(std::make_pair(id, a));
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
               ogdfNdMap[n.second.id] = ogdfNd;
               nodeVec[ogdfNd->index()] = &n.second;
            }

            std::vector<std::pair<ogdf::edge, GraphArc<GT>*>> edges;
            for (auto& a : arcMap_)
            {
               const std::string& id0 = a.second.n0->id;
               const std::string& id1 = a.second.n1->id;
               ogdf::edge e = g.newEdge(ogdfNdMap[id0], ogdfNdMap[id1]);
               edges.push_back(std::make_pair(e, &(a.second)));
            }

            ogdf::EdgeArray<double> ea(g);
            for (auto& elem : edges)
            {
               ea[elem.first] = elem.second->info.l;
            }

            ogdf::node n;
            forall_nodes(n, g)
            {
               ga.width(n) = nodeVec[n->index()]->info.w;
               ga.height(n) = nodeVec[n->index()]->info.h;
            }

            layoutOgdf(g, ga, ea, 0);

            forall_nodes(n, g)
            {
               int idx = n->index();
               nodeVec[idx]->info.x = ga.x(n);
               nodeVec[idx]->info.y = ga.y(n);
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

   extern template class NetworkGraph<>;

} // namespace SmartGridToolbox

#endif // NETWORKGRAPH_H
