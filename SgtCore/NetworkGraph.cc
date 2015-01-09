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
   void NetworkGraph::addArc(const std::string& id, const std::string& id0, const std::string& id1,
         std::unique_ptr<NetworkArcInfo> info)
   {
      auto& n0 = nodeMap_.at(id0);
      auto& n1 = nodeMap_.at(id1);
      auto a = NetworkArc(id, &n0, &n1, std::move(info));
      n0.adjacentArcs.push_back(&a);
      arcMap_.emplace(std::make_pair(id, std::move(a)));
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
      forall_nodes(n, g)
      {
         if (ga.x(n) < xmin) xmin = ga.x(n);
         if (ga.y(n) < ymin) ymin = ga.y(n);
         if (ga.x(n) > xmax) xmax = ga.x(n);
         if (ga.y(n) > ymax) ymax = ga.y(n);
      }
      double xCenter = 0.5 * (xmin + xmax);
      double yCenter = 0.5 * (ymin + ymax);

      forall_nodes(n, g)
      {
         int idx = n->index();
         nodeVec[idx]->info->x = ga.x(n) - xCenter;
         nodeVec[idx]->info->y = ga.y(n) - yCenter;
      }
   }
}
