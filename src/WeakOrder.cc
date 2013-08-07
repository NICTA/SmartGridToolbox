#include "Common.h"
#include "WeakOrder.h"
#include <iostream>
#include <algorithm>
#include <sstream>
using namespace std;

std::string prlevel(int level)
{
   std::stringstream ss;
   for (int i = 0; i < level; ++i) {ss << "\t";}
   return ss.str();
}

namespace SmartGridToolbox
{
   void WoNode::dfs(std::vector<WoNode *> & stack)
   {
      static int level = 0;
      ++level;

      if (!visited_)
      {
         visited_ = true;
         SGT_DEBUG(debug() << prlevel(level - 1) << "DFS " << idx_ << " {" << std::endl);
         for (WoNode * predecessor : stack)
         {
            SGT_DEBUG(debug() << prlevel(level) << predecessor->idx_ << std::endl);
            predecessor->descendents_.insert(this);
         }
         stack.push_back(this);
         for (WoNode * toNd : to_)
         {
            toNd->dfs(stack);
         }
         stack.pop_back();
         SGT_DEBUG(debug() << prlevel(level - 1) << "}" << std::endl);
      }
      --level;
   }

   WoGraph::WoGraph(int n) : n_(n), nodes_(n)
   {
      for (int i = 0; i < n; ++i)
      {
         nodes_[i] = new WoNode(i);
      }
   }

   WoGraph::~WoGraph()
   {
      for (WoNode * nd : nodes_)
      {
         delete nd;
      }
   }

   void WoGraph::weakOrder()
   {
      std::vector<WoNode *> stack;
      // First do a DFS to induce an order on the nodes.
      bool done = false; 
      done = true;
      for (WoNode * nd : nodes_)
      {
         nd->dfs(stack);
         for (WoNode * nd : nodes_)
         {
            nd->visited_ = false;
         }
      }

      // Now sort and rank the nodes.

      // First calculate dominance.
      // A dominates B -> A precedes B but not vice versa. 
      for (WoNode * nd : nodes_)
      {
         for (const WoNode * nd2 : nd->descendents_)
         {
            if (nd2->descendents_.find(nd) == nd2->descendents_.end())
            {
               nd->dominated_.insert(nd2);
            }
         }
      }

      // Sort the nodes, based on domination. 
      std::sort(nodes_.begin(), nodes_.end(),
            [](const WoNode * lhs, const WoNode * rhs) -> bool
            {return *rhs < *lhs;});
   }
}
