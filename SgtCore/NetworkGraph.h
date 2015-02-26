#ifndef NETWORKGRAPH_H
#define NETWORKGRAPH_H

#include <libcola/cola.h>

#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace
{
   inline double myRand(double l)
   {
      static std::default_random_engine re;      
      std::uniform_real_distribution<double> unif(-0.5 * l, 0.5 * l);
      return unif(re);
   }
}

namespace SmartGridToolbox
{
   struct BasicGraphNodeInfo
   {
      double x;
      double y;
      double w;
      double h;
      bool locked;
      int idx; // Convenience, may not be used... 
   };
   
   struct BasicGraphArcInfo
   {
      double l;
      bool ignore;
   };

   template<typename NI = BasicGraphNodeInfo, typename AI = BasicGraphArcInfo> struct GraphArc;

   template<typename NI = BasicGraphNodeInfo, typename AI = BasicGraphArcInfo> struct GraphNode
   {
      std::string id;
      std::vector<GraphArc<NI, AI>*> adjacentArcs;
      NI info;

      GraphNode(const std::string& id, const NI& info) : 
         id(id), info(info)
      {
         // Empty.
      }
   };

   template<typename NI, typename AI> struct GraphArc
   {
      std::string id;
      const GraphNode<NI, AI>* n0;
      const GraphNode<NI, AI>* n1;
      AI info;
      
      GraphArc(const std::string& id, const GraphNode<NI, AI>* n0, const GraphNode<NI, AI>* n1, const AI& info) :
         id(id), n0(n0), n1(n1), info(info) 
      {
         // Empty.
      }
   };
   
   template<typename NI = BasicGraphNodeInfo, typename AI = BasicGraphArcInfo> class NetworkGraph
   {
      public:

         void addNode(const std::string& id, const NI& info)
         {
            nodeMap_.emplace(std::make_pair(id, GraphNode<NI, AI>{id, info}));
         }

         void addArc(const std::string& id, const std::string& id0, const std::string& id1, const AI& info)
         {
            auto& n0 = nodeMap_.at(id0);
            auto& n1 = nodeMap_.at(id1);
            auto a = GraphArc<NI, AI>(id, &n0, &n1, info);
            n0.adjacentArcs.push_back(&a);
            arcMap_.emplace(std::make_pair(id, a));
         }
         
         void randomize(double width, double height)
         {
            for (auto& n : nodeMap_)
            {
               n.second.info.x = myRand(width);
               n.second.info.y = myRand(height);
            }
         }

         const std::map<std::string, GraphNode<NI, AI>>& nodes() const
         {
            return nodeMap_;
         }
         
         std::map<std::string, GraphNode<NI, AI>>& nodes()
         {
            return nodeMap_;
         }

         const std::map<std::string, GraphArc<NI, AI>>& arcs() const
         {
            return arcMap_;
         }
         
         std::map<std::string, GraphArc<NI, AI>>& arcs()
         {
            return arcMap_;
         }

         void layout(int maxIter = 100)
         {
            // TODO: It would be nice to have a "step layout" function, but for some reason this appears to
            // not satisfy node separation constraints for the rectangles. Maybe there is some way to do e.g. using
            // the makeFeasible member function? If so, we should return a double (the stress) from this function and
            // add a bool argument saying whether we want only one or many steps.
            std::vector<vpsc::Rectangle*> rs;
            std::vector<cola::Edge> es;
            std::vector<double> ls;

            int i = 0;
            for (auto& n : nodeMap_)
            {
               rs.push_back(new vpsc::Rectangle(
                        n.second.info.x - 0.5 * n.second.info.w, n.second.info.x + 0.5 * n.second.info.w,
                        n.second.info.y - 0.5 * n.second.info.h, n.second.info.y + 0.5 * n.second.info.h));
               n.second.info.idx = i++;
            }

            for (auto& a : arcMap_)
            {
               if (!a.second.info.ignore)
               {
                  auto i0 = a.second.n0->info.idx;
                  auto i1 = a.second.n1->info.idx;
                  es.push_back(std::make_pair(i0, i1));
                  ls.push_back(a.second.info.l);
               }
            }

            cola::Locks locks;
            for (auto& n : nodeMap_)
            {
               if (n.second.info.locked)
               {
                  locks.push_back(cola::Lock(n.second.info.idx, n.second.info.x, n.second.info.y));
               }
            }
            cola::TestConvergence testConv(1e-4, maxIter);
            cola::PreIteration preIter(locks);
            cola::ConstrainedFDLayout fd(rs, es, 1, true, ls, &testConv, &preIter);
            fd.run();

            i = 0;
            for (auto& n : nodeMap_)
            {
               n.second.info.x = rs[i]->getCentreX();
               n.second.info.y = rs[i]->getCentreY();
               ++i;
            }

            for (auto r : rs)
            {
               delete r;
            }
         }

      private:

         std::map<std::string, GraphNode<NI, AI>> nodeMap_;
         std::map<std::string, GraphArc<NI, AI>> arcMap_;
   };
} // namespace SmartGridToolbox

#endif // NETWORKGRAPH_H
