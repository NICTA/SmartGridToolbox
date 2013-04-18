#include "WeakOrder.h"
#include <iostream>
#include <algorithm>
using namespace std;

void prlevel(int level)
{
   for (int i = 0; i < level; ++i) {cout << "      ";}
}

namespace SmartGridToolbox
{
   void WONode::DFS(std::vector<WONode *> & stack)
   {
      static int level = 0;
      ++level;

      if (!visited_)
      {
         visited_ = true;
         prlevel(level - 1); cout << "DFS " << idx_ << " {" << endl;
         for (WONode * predecessor : stack)
         {
            prlevel(level); cout << predecessor->idx_ << " precedes " << idx_ << endl;
            predecessor->descendents_.insert(this);
         }
         stack.push_back(this);
         for (WONode * toNd : to_)
         {
            toNd->DFS(stack);
         }
         stack.pop_back();
         prlevel(level - 1); cout << "}" << endl;
      }
      --level;
   }

   WOGraph::WOGraph(int n) : n_(n), nodes_(n)
   {
      for (int i = 0; i < n; ++i)
      {
         nodes_[i] = new WONode(i);
      }
   }

   WOGraph::~WOGraph()
   {
      for (WONode * nd : nodes_)
      {
         delete nd;
      }
   }

   void WOGraph::WeakOrder()
   {
      std::vector<WONode *> stack;
      // First do a DFS to induce an order on the nodes.
      bool done = false; 
      done = true;
      for (WONode * nd : nodes_)
      {
         nd->DFS(stack);
         for (WONode * nd : nodes_)
         {
            nd->visited_ = false;
         }
      }

      // Now sort and rank the nodes.

      // First calculate dominance.
      // A dominates B -> A precedes B but not vice versa. 
      for (WONode * nd : nodes_)
      {
         for (const WONode * nd2 : nd->descendents_)
         {
            if (nd2->descendents_.find(nd) == nd2->descendents_.end())
            {
               nd->dominated_.insert(nd2);
            }
         }
      }

      // Sort the nodes, based on domination. 
      std::sort(nodes_.begin(), nodes_.end(),
            [](const WONode * lhs, const WONode * rhs) -> bool
            {return *rhs < *lhs;});
   }
}
