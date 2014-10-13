#include "WeakOrder.h"

#include <SgtCore/Common.h>

#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;

std::string prlevel(int level)
{
   std::stringstream ss;
   for (int i = 0; i < level; ++i) {ss << "    ";}
   return ss.str();
}

namespace SmartGridToolbox
{
   void WoNode::dfs(std::vector<WoNode*>& stack)
   {
      static int level = 0;
      ++level;

      if (!visited_)
      {
         visited_ = true;
         SGT_DEBUG(Log().debug() << prlevel(level - 1) << "DFS " << idx_ << " {" << std::endl);
         for (WoNode* predecessor : stack)
         {
            SGT_DEBUG(Log().debug() << prlevel(level) << predecessor->idx_ << std::endl);
            predecessor->descendents_.insert(this);
         }
         stack.push_back(this);
         for (WoNode* toNd : to_)
         {
            toNd->dfs(stack);
         }
         stack.pop_back();
         SGT_DEBUG(Log().debug() << prlevel(level - 1) << "}" << std::endl);
      }
      --level;
   }

   WoGraph::WoGraph(int n) : nodes_(n)
   {
      for (int i = 0; i < n; ++i)
      {
         nodes_[i] = std::unique_ptr<WoNode>(new WoNode(i));
      }
   }

   void WoGraph::weakOrder()
   {
      SGT_DEBUG(Log().debug() << std::endl);
      SGT_DEBUG(
            Log().debug() << "Weak order graph: initial:" << std::endl;
            debugPrint();
            Log().debug() << std::endl;
            );

      std::vector<WoNode*> stack;
      // First do a DFS to induce an order on the nodes.
      bool done = false;
      done = true;
      for (const std::unique_ptr<WoNode>& nd : nodes_)
      {
         nd->dfs(stack);
         for (const std::unique_ptr<WoNode>& nd : nodes_)
         {
            nd->visited_ = false;
         }
      }

      // Now sort and rank the nodes.

      // First calculate dominance.
      // A dominates B -> A precedes B but not vice versa.
      for (const std::unique_ptr<WoNode>& nd : nodes_)
      {
         for (const WoNode* nd2 : nd->descendents_)
         {
            if (nd2->descendents_.find(nd.get()) == nd2->descendents_.end())
            {
               nd->dominated_.insert(nd2);
            }
         }
      }

      // Sort the nodes, based on <. Note that it isn't a strict weak ordering, so can't use stl sort.
      // TODO: This is probably a very dumb kind of sort!
      // Also note that this sort will, as far as possible, preserve features of the original ordering, 
      // so if we want e.g. a secondary alphabetical ordering then we should first pass in an alphabetically ordered
      // vector.

      SGT_DEBUG(
            Log().debug() << "Weak order graph: pre-sort:" << std::endl;
            debugPrint();
            Log().debug() << std::endl;
            );

      for (auto it1 = nodes_.begin(); it1 != nodes_.end(); ++it1)
      {
         for (auto it2 = it1 + 1; it2 != nodes_.end(); ++it2)
         {
            if ((**it2).dominates(**it1))
            {
               auto it3 = it2;
               while (it3 != it1)
               {
                  std::swap(*it3, *(it3 - 1));
                  --it3;
               }
            }
         }
      }

      SGT_DEBUG(Log().debug() << "Weak order graph: final:" << std::endl; debugPrint());
   }

   void WoGraph::debugPrint()
   {
      Log().debug() << "Weak order graph: Node i : direct descendents" << std::endl;
      for (const std::unique_ptr<WoNode>& nd1 : nodes())
      {
         auto log = Log();
         log.debug() << nd1->index() << "   ";
         for (const WoNode* nd2 : nd1->to_)
         {
            log.debug() << nd2->index() << " ";
         }
         log.debug() << endl;
      }
      Log().debug() << endl;
      Log().debug() << "Weak order graph: Node i : indirect descendents" << std::endl;
      {
         auto log = Log();
         for (const std::unique_ptr<WoNode>& nd1 : nodes())
         {
            log.debug() << nd1->index() << "   ";
            for (const WoNode* nd2 : nd1->descendents_)
            {
               log.debug() << nd2->index() << " ";
            }
            log.debug() << endl;
         }
      }
      Log().debug() << endl;
      Log().debug() << "Weak order graph: Node i : dominated nodes" << std::endl;
      {
         auto log = Log();
         for (const std::unique_ptr<WoNode>& nd1 : nodes())
         {
            log.debug() << nd1->index() << "   ";
            for (const WoNode* nd2 : nd1->dominated_)
            {
               log.debug() << nd2->index() << " ";
            }
            log.debug() << endl;
         }
      }
      Log().debug() << endl;
      Log().debug() << "Weak order graph: (i, j) : i dominates j" << std::endl;
      {
         auto log = Log();
         log.debug() << "     ";
         for (const std::unique_ptr<WoNode>& nd2 : nodes())
         {
            log.debug() << setw(3) << left << nd2->index();
         }
         log.debug() << endl; 
      }
      Log().debug() << endl; 
      for (const std::unique_ptr<WoNode>& nd1 : nodes())
      {
         auto log = Log();
         log.debug() << setw(5) << left << nd1->index();
         for (const std::unique_ptr<WoNode>& nd2 : nodes())
         {
            log.debug() << setw(3) << left << (nd1->dominates(*nd2));
         }
         log.debug() << endl;
      }
      Log().debug() << endl; 
      Log().debug() << "Weak order graph: (i, j) : i < j" << std::endl;
      {
         auto log = Log();
         log.debug() << "     ";
         for (const std::unique_ptr<WoNode>& nd2 : nodes())
         {
            log.debug() << setw(3) << left << nd2->index();
         }
         log.debug() << endl;
      }
      Log().debug() << endl; 
      for (const std::unique_ptr<WoNode>& nd1 : nodes())
      {
         auto log = Log();
         Log().debug() << setw(5) << left << nd1->index();
         for (const std::unique_ptr<WoNode>& nd2 : nodes())
         {
            log.debug() << setw(3) << left << (*nd1 < *nd2);
         }
         log.debug() << endl;
      }
   }
}
