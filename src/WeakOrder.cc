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
      in_stack_ = true;
      static int level = 0;
      ++level;
      prlevel(level - 1); cout << "DFS " << idx_ << " {" << endl;
      if (!finished_)
      {
         bool all_finished = true;
         visited_ = true;
         for (WONode * nd : to_)
         {
            if (!nd->in_stack_ && !nd->finished_)
            {
               nd->DFS(stack);
               if (nd->finished_)
               {
                  nd->just_finished_ = true;
               }
               else
               {
                  all_finished = false;
               }
            }
         }
         for (WONode * nd : to_)
         {
            descendents_.insert(nd);
            prlevel(level); cout << "->   " << nd->idx_ << endl;
            if (!nd->in_stack_)
            {
               for (const WONode * ndb : nd->descendents_)
               {
                  descendents_.insert(ndb);
                  prlevel(level); cout << "->-> " << ndb->idx_ << endl;
               }
            }
            nd->just_finished_ = false;
         }
         finished_ = all_finished;
      }
      prlevel(level - 1); cout << "} " << finished_ << endl;
      --level;
      in_stack_ = false;
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
      while (!done)
      {
         done = true;
         for (WONode * nd : nodes_)
         {
            nd->DFS(stack);
            if (!nd->finished_)
            {
               done = false;
            }
         }
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
