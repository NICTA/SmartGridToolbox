#include "NetworkGraph.h"

#include <ogdf/misclayout/BalloonLayout.h>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/energybased/FMMMLayout.h>
#include <ogdf/energybased/SpringEmbedderFR.h>
#include <ogdf/planarity/PlanarizationLayout.h>
#include <ogdf/planarity/VariableEmbeddingInserter.h>
#include <ogdf/planarity/FastPlanarSubgraph.h>
#include <ogdf/orthogonal/OrthoLayout.h>
#include <ogdf/planarity/EmbedderMinDepthMaxFaceLayers.h>
#include <ogdf/layered/SugiyamaLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/FastHierarchyLayout.h>

namespace SmartGridToolbox
{

   NetworkNode::NetworkNode(const std::string& id, double x, double y, double w, double h) :
      id(id),
      x(x),
      y(y),
      w(w),
      h(h)
   {
      // Empty.
   }
      
   NetworkArc::NetworkArc(const std::string& id, double l, 
         std::unique_ptr<NetworkNode> n0, std::unique_ptr<NetworkNode> n1) :
      id(id),
      l(l),
      n0(n0),
      n1(n1)
   {
      // Empty.
   }

   void addArc(std::unique_ptr<NetworkArc> a, std::string )
   {
      auto n0 = nodeMap_.at(id0);
      auto n1 = nodeMap_.at(id1);
      auto e = arcMap_[id] = new NetworkArc{id, l, n0,  n1};
      n0->adjacentArcs.push_back(e);
   }

   void NetworkGraph::layout()
   {
      ogdf::Graph g;
      ogdf::GraphAttributes ga(g, ogdf::GraphAttributes::nodeGraphics | ogdf::GraphAttributes::edgeGraphics);

      std::map<std::string, ogdf::node> ogdfNdMap;
      std::vector<NetworkNode*> nodeVec(nodeMap_.size());

      for (auto& n : nodeMap_)
      {
         ogdf::node ogdfNd = g.newNode();
         ogdfNdMap.at(n.second->id) = ogdfNd;
         nodeVec[ogdfNd->index()] = n.second;
      }

      for (auto& e : arcMap_)
      {
         const std::string& id0 = e.second->n0->id;
         const std::string& id1 = e.second->n1->id;
         g.newEdge(ogdfNdMap[id0], ogdfNdMap[id1]);
      }

      ogdf::node v;
      forall_nodes(v, g)
      {
         ga.width(v) = nodeVec[v->index()]->w;
         ga.height(v) = nodeVec[v->index()]->h;
      }

      int layoutType = 0;

      if (layoutType == 0)
      {
         ogdf::FMMMLayout fmmm;
         fmmm.repulsiveForcesCalculation(ogdf::FMMMLayout::rfcNMM);
         fmmm.useHighLevelOptions(true);
         fmmm.unitEdgeLength(15.0);
         fmmm.newInitialPlacement(true);
         fmmm.qualityVersusSpeed(ogdf::FMMMLayout::qvsGorgeousAndEfficient);
         fmmm.call(ga);
      }
      else if (layoutType == 1)
      {
         ogdf::PlanarizationLayout pl;
         auto ps = new ogdf::FastPlanarSubgraph;
         ps->runs(100);
         auto ves = new ogdf::VariableEmbeddingInserter;
         ves->removeReinsert(ogdf::EdgeInsertionModule::rrAll);
         pl.setSubgraph(ps);
         pl.setInserter(ves);

         auto emb = new ogdf::EmbedderMinDepthMaxFaceLayers;
         pl.setEmbedder(emb);

         auto ol = new ogdf::OrthoLayout;
         ol->separation(20.0);
         ol->cOverhang(0.4);
         ol->setOptions(2+4);
         pl.setPlanarLayouter(ol);

         pl.call(ga);
      }
      else if (layoutType == 2)
      {
         ogdf::SugiyamaLayout sl;
         sl.setRanking(new ogdf::OptimalRanking);
         sl.setCrossMin(new ogdf::MedianHeuristic);

         auto fhl = new ogdf::FastHierarchyLayout;
         fhl->layerDistance(10.0);
         fhl->nodeDistance(10.0);
         sl.setLayout(fhl);

         sl.call(ga);
      }
      else if (layoutType == 3)
      {
         ogdf::SpringEmbedderFR frl;
         frl.call(ga);
      }

      double xmin = std::numeric_limits<double>::max();
      double ymin = std::numeric_limits<double>::max();
      double xmax = std::numeric_limits<double>::min();
      double ymax = std::numeric_limits<double>::min();
      forall_nodes(v, g)
      {
         if (ga.x(v) < xmin) xmin = ga.x(v);
         if (ga.y(v) < ymin) ymin = ga.y(v);
         if (ga.x(v) > xmax) xmax = ga.x(v);
         if (ga.y(v) > ymax) ymax = ga.y(v);
      }
      double xCenter = 0.5 * (xmin + xmax);
      double yCenter = 0.5 * (ymin + ymax);

      forall_nodes(v, g)
      {
         int idx = v->index();
         nodeVec[idx]->x = ga.x(v) - xCenter;
         nodeVec[idx]->y = ga.y(v) - yCenter;
      }
   }
}
