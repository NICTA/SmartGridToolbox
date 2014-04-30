#include <SmartGridToolbox/Common.h>
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
   void WoNode::dfs(std::vector<WoNode*> & stack)
   {
      static int level = 0;
      ++level;

      if (!visited_)
      {
         visited_ = true;
         SGT_DEBUG(debug() << prlevel(level - 1) << "DFS " << idx_ << " {" << std::endl);
         for (WoNode* predecessor : stack)
         {
            SGT_DEBUG(debug() << prlevel(level) << predecessor->idx_ << std::endl);
            predecessor->descendents_.insert(this);
         }
         stack.push_back(this);
         for (WoNode* toNd : to_)
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
         nodes_[i] = std::unique_ptr<WoNode>(new WoNode(i));
      }
   }

   void WoGraph::weakOrder()
   {
      SGT_DEBUG (debug() << "Weak order graph: initial:" << std::endl; debugPrint());

      std::vector<WoNode*> stack;
      // First do a DFS to induce an order on the nodes.
      bool done = false;
      done = true;
      for (const std::unique_ptr<WoNode> & nd : nodes_)
      {
         nd->dfs(stack);
         for (const std::unique_ptr<WoNode> & nd : nodes_)
         {
            nd->visited_ = false;
         }
      }

      // Now sort and rank the nodes.

      // First calculate dominance.
      // A dominates B -> A precedes B but not vice versa.
      for (const std::unique_ptr<WoNode> & nd : nodes_)
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

      SGT_DEBUG (debug() << "Weak order graph: pre-sort:" << std::endl; debugPrint());

      for (auto it1 = nodes_.begin(); it1 != nodes_.end(); ++it1)
      {
         for (auto it2 = it1 + 1; it2 != nodes_.end(); ++it2)
         {
            SGT_DEBUG(
                  debug() << std::setw(3) << (**it1).index() << " " << std::setw(3) << (**it2).index() << " A ";
                  for (auto it4 = nodes_.begin(); it4 != nodes_.end(); ++it4)
                  {
                     debugStream() << std::setw(3) << (**it4).index() << " ";
                  }
                  debugStream() << std::endl;);

            if ((**it2).dominates(**it1))
            {
               auto it3 = it2;
               while (it3 != it1)
               {
                  std::swap(*it3, *(it3 - 1));
                  --it3;
                  SGT_DEBUG(
                        debug() << std::setw(3) << (**it1).index() << " " << std::setw(3) << (**it2).index() << " A ";
                        for (auto it4 = nodes_.begin(); it4 != nodes_.end(); ++it4)
                        {
                           debugStream() << std::setw(3) << (**it4).index() << " ";
                        }
                        debugStream() << std::endl;);
               }
            }
         }
      }

      SGT_DEBUG (debug() << "Weak order graph: final:" << std::endl; debugPrint());
   }

   void WoGraph::debugPrint()
   {
      debug() << "Weak order graph: Node i : direct descendents" << std::endl;
      for (const std::unique_ptr<WoNode> & nd1 : nodes())
      {
         debug() << nd1->index() << "   ";
         for (const WoNode* nd2 : nd1->to_)
         {
            debugStream() << nd2->index() << " ";
         }
         debugStream() << endl;
      }
      debugStream() << endl;

      debug() << "Weak order graph: Node i : indirect descendents" << std::endl;
      for (const std::unique_ptr<WoNode> & nd1 : nodes())
      {
         debug() << nd1->index() << "   ";
         for (const WoNode* nd2 : nd1->descendents_)
         {
            debugStream() << nd2->index() << " ";
         }
         debugStream() << endl;
      }
      debugStream() << endl;

      debug() << "Weak order graph: Node i : dominated nodes" << std::endl;
      for (const std::unique_ptr<WoNode> & nd1 : nodes())
      {
         debug() << nd1->index() << "   ";
         for (const WoNode* nd2 : nd1->dominated_)
         {
            debugStream() << nd2->index() << " ";
         }
         debugStream() << endl;
      }
      debugStream() << endl;

      debug() << "Weak order graph: (i, j) : i dominates j" << std::endl;
      debug() << "     ";
      for (const std::unique_ptr<WoNode> & nd2 : nodes())
      {
         debugStream() << setw(3) << left << nd2->index();
      }
      debugStream() << endl << endl;
      for (const std::unique_ptr<WoNode> & nd1 : nodes())
      {
         debug() << setw(5) << left << nd1->index();
         for (const std::unique_ptr<WoNode> & nd2 : nodes())
         {
            debugStream() << setw(3) << left << (nd1->dominates(*nd2));
         }
         debugStream() << endl;
      }

      debug() << "Weak order graph: (i, j) : i < j" << std::endl;
      debug() << "     ";
      for (const std::unique_ptr<WoNode> & nd2 : nodes())
      {
         debugStream() << setw(3) << left << nd2->index();
      }
      debugStream() << endl << endl;
      for (const std::unique_ptr<WoNode> & nd1 : nodes())
      {
         debug() << setw(5) << left << nd1->index();
         for (const std::unique_ptr<WoNode> & nd2 : nodes())
         {
            debugStream() << setw(3) << left << (*nd1 < *nd2);
         }
         debugStream() << endl;
      }
   }
}
