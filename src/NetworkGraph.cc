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

using namespace ogdf;
using OgdfNode = ogdf::node;

void NetworkGraph::init(const SmartGridToolbox::Network& netw)
{
   for (auto sgtNode : netw.nodes())
   {
      addNode(sgtNode.get());
   }
   for (auto sgtArc : netw.arcs())
   {
      addEdge(sgtArc.get());
   }
   validate();
   layout();
}

void NetworkGraph::layout()
{
   Graph g;
   GraphAttributes ga(g, GraphAttributes::nodeGraphics | GraphAttributes::edgeGraphics );

   std::map<std::string, OgdfNode> ogdfNdMap;
   std::vector<NetworkNode*> ndVec(nodeMap_.size());

   for (auto& nd : nodeMap_)
   {
      OgdfNode ogdfNd = g.newNode();
      ogdfNdMap[nd.second.sgtNode->bus()->id()] = ogdfNd;
      ndVec[ogdfNd->index()] = &nd.second;
   }

   for (auto& e : edgeVec_)
   {
      const std::string& id0 = e.sgtArc->node0()->bus()->id();
      const std::string& id1 = e.sgtArc->node1()->bus()->id();
      edge a = g.newEdge(ogdfNdMap[id0], ogdfNdMap[id1]);
   }

   OgdfNode v;
   forall_nodes(v, g)
   {
      ga.width(v) = ga.height(v) = 20.0;
   }
  
   int layoutType = 0;

   if (layoutType == 0)
   {
      FMMMLayout fmmm;
      fmmm.repulsiveForcesCalculation(FMMMLayout::rfcNMM);
      fmmm.useHighLevelOptions(true);
      fmmm.unitEdgeLength(15.0);
      fmmm.newInitialPlacement(true);
      fmmm.qualityVersusSpeed(FMMMLayout::qvsGorgeousAndEfficient);
      fmmm.call(ga);
   }
   else if (layoutType == 1)
   {
      PlanarizationLayout pl;
      FastPlanarSubgraph *ps = new FastPlanarSubgraph;
      ps->runs(100);
      VariableEmbeddingInserter *ves = new VariableEmbeddingInserter;
      ves->removeReinsert(EdgeInsertionModule::rrAll);
      pl.setSubgraph(ps);
      pl.setInserter(ves);

      EmbedderMinDepthMaxFaceLayers *emb = new EmbedderMinDepthMaxFaceLayers;
      pl.setEmbedder(emb);

      OrthoLayout *ol = new OrthoLayout;
      ol->separation(20.0);
      ol->cOverhang(0.4);
      ol->setOptions(2+4);
      pl.setPlanarLayouter(ol);

      pl.call(ga);
   }
   else if (layoutType == 2)
   {
      SugiyamaLayout sl;
      sl.setRanking(new OptimalRanking);
      sl.setCrossMin(new MedianHeuristic);

      FastHierarchyLayout *fhl = new FastHierarchyLayout;
      fhl->layerDistance(10.0);
      fhl->nodeDistance(10.0);
      sl.setLayout(fhl);

      sl.call(ga);
   }
   else if (layoutType == 3)
   {
      SpringEmbedderFR frl;
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
      ndVec[idx]->x = ga.x(v) - xCenter;
      ndVec[idx]->y = ga.y(v) - yCenter;
   }
}

void NetworkGraph::addNode(SmartGridToolbox::Node* sgtNode)
{
   nodeMap_[sgtNode->bus()->id()] = {sgtNode};
}

void NetworkGraph::addEdge(SmartGridToolbox::Arc* sgtArc)
{
   edgeVec_.push_back({sgtArc});
}

void NetworkGraph::validate()
{
   for (auto& e : edgeVec_)
   {
      auto id0 = e.sgtArc->node0()->bus()->id();
      auto id1 = e.sgtArc->node1()->bus()->id();
      auto& nd0 = nodeMap_.at(id0);
      auto& nd1 = nodeMap_.at(id1);
      nd0.adjacentEdges.push_back(&e);
      nd1.adjacentEdges.push_back(&e);
      e.nd0 = &nd0;
      e.nd1 = &nd1;
   }
}
