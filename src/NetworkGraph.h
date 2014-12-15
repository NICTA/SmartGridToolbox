#ifndef NETWORKGRAPH_H
#define NETWORKGRAPH_H

#include <SgtCore.h>
#include <SgtSim.h>

#include <map>
#include <string>
#include <vector>

struct NetworkEdge;

struct NetworkNode
{
   SmartGridToolbox::Node* sgtNode;
   std::vector<NetworkEdge*> adjacentEdges;
   double x;
   double y;
};

struct NetworkEdge
{
   SmartGridToolbox::Arc* sgtArc;
   NetworkNode* nd0;
   NetworkNode* nd1;
};

class NetworkGraph
{
   public:
      void init(const SmartGridToolbox::Network& netw);
      void layout();

      const std::map<std::string, NetworkNode>& nodes() const {return nodeMap_;}
      const std::vector<NetworkEdge>& edges() const {return edgeVec_;}

   private:
      void addNode(SmartGridToolbox::Node* sgtNode);
      void addEdge(SmartGridToolbox::Arc *sgtArc);
      void validate();

   private:
      std::map<std::string, NetworkNode> nodeMap_;
      std::vector<NetworkEdge> edgeVec_;
};

#endif // NETWORKGRAPH_H
