// Copyright 2015-2016 National ICT Australia Limited (NICTA)
// Copyright 2016-2019 The Australian National University
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "WeakOrder.h"

#include <SgtCore/Common.h>

#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;

std::string prlevel(unsigned int level)
{
    std::stringstream ss;
    for (unsigned int i = 0; i < level; ++i) {ss << "    ";}
    return ss.str();
}

namespace Sgt
{
    void WoNode::dfs(std::vector<WoNode*>& stack)
    {
        static unsigned int level = 0;
        ++level;

        if (!visited_)
        {
            visited_ = true;
            sgtLogDebug(LogLevel::VERBOSE) << prlevel(level - 1) << "DFS " << idx_ << " {" << std::endl;
            for (WoNode* predecessor : stack)
            {
                sgtLogDebug(LogLevel::VERBOSE) << prlevel(level) << predecessor->idx_ << std::endl;
                predecessor->descendents_.insert(this);
            }
            stack.push_back(this);
            for (WoNode* toNd : to_)
            {
                toNd->dfs(stack);
            }
            stack.pop_back();
            sgtLogDebug(LogLevel::VERBOSE) << prlevel(level - 1) << "}" << std::endl;
        }
        --level;
    }

    WoGraph::WoGraph(std::size_t n) : nodes_(n)
    {
        for (std::size_t i = 0; i < n; ++i)
        {
            nodes_[i] = std::make_unique<WoNode>(i);
        }
    }

    void WoGraph::weakOrder()
    {
        if (debugLogLevel() >= LogLevel::NORMAL)
        {
            sgtLogDebug(LogLevel::VERBOSE) << std::endl;
            sgtLogDebug(LogLevel::VERBOSE) << "Weak order graph: initial:" << std::endl;
            debugPrint();
            sgtLogDebug(LogLevel::VERBOSE) << std::endl;
        }

        std::vector<WoNode*> stack;
        // First do a DFS to induce an order on the nodes.
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

        if (debugLogLevel() >= LogLevel::VERBOSE)
        {
            sgtLogDebug(LogLevel::VERBOSE) << "Weak order graph: pre-sort:" << std::endl;
            debugPrint();
            sgtLogDebug(LogLevel::VERBOSE) << std::endl;
        }

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

        sgtLogDebug(LogLevel::VERBOSE) << "Weak order graph: final:" << std::endl; debugPrint();
    }

    void WoGraph::debugPrint()
    {
        // TODO: Indentation may be messed up here.
        sgtLogDebug(LogLevel::VERBOSE) << "Weak order graph: Node i : direct descendents" << std::endl;
        for (const std::unique_ptr<WoNode>& nd1 : nodes())
        {
            sgtLogDebug(LogLevel::VERBOSE) << nd1->index() << "   ";
            for (const WoNode* nd2 : nd1->to_)
            {
                sgtLogDebug(LogLevel::VERBOSE) << nd2->index() << " ";
            }
            sgtLogDebug(LogLevel::VERBOSE) << endl;
        }
        sgtLogDebug(LogLevel::VERBOSE) << endl;
        sgtLogDebug(LogLevel::VERBOSE) << "Weak order graph: Node i : indirect descendents" << std::endl;
        {
            for (const std::unique_ptr<WoNode>& nd1 : nodes())
            {
                sgtLogDebug(LogLevel::VERBOSE) << nd1->index() << "   ";
                for (const WoNode* nd2 : nd1->descendents_)
                {
                    sgtLogDebug(LogLevel::VERBOSE) << nd2->index() << " ";
                }
                sgtLogDebug(LogLevel::VERBOSE) << endl;
            }
        }
        sgtLogDebug(LogLevel::VERBOSE) << endl;
        sgtLogDebug(LogLevel::VERBOSE) << "Weak order graph: Node i : dominated nodes" << std::endl;
        {
            for (const std::unique_ptr<WoNode>& nd1 : nodes())
            {
                sgtLogDebug(LogLevel::VERBOSE) << nd1->index() << "   ";
                for (const WoNode* nd2 : nd1->dominated_)
                {
                    sgtLogDebug(LogLevel::VERBOSE) << nd2->index() << " ";
                }
                sgtLogDebug(LogLevel::VERBOSE) << endl;
            }
        }
        sgtLogDebug(LogLevel::VERBOSE) << endl;
        sgtLogDebug(LogLevel::VERBOSE) << "Weak order graph: (i, j) : i dominates j" << std::endl;
        {
            sgtLogDebug(LogLevel::VERBOSE) << "     ";
            for (const std::unique_ptr<WoNode>& nd2 : nodes())
            {
                sgtLogDebug(LogLevel::VERBOSE) << setw(3) << left << nd2->index();
            }
            sgtLogDebug(LogLevel::VERBOSE) << endl;
        }
        sgtLogDebug(LogLevel::VERBOSE) << endl;
        for (const std::unique_ptr<WoNode>& nd1 : nodes())
        {
            sgtLogDebug(LogLevel::VERBOSE) << setw(5) << left << nd1->index();
            for (const std::unique_ptr<WoNode>& nd2 : nodes())
            {
                sgtLogDebug(LogLevel::VERBOSE) << setw(3) << left << (nd1->dominates(*nd2));
            }
            sgtLogDebug(LogLevel::VERBOSE) << endl;
        }
        sgtLogDebug(LogLevel::VERBOSE) << endl;
        sgtLogDebug(LogLevel::VERBOSE) << "Weak order graph: (i, j) : i < j" << std::endl;
        {
            sgtLogDebug(LogLevel::VERBOSE) << "     ";
            for (const std::unique_ptr<WoNode>& nd2 : nodes())
            {
                sgtLogDebug(LogLevel::VERBOSE) << setw(3) << left << nd2->index();
            }
            sgtLogDebug(LogLevel::VERBOSE) << endl;
        }
        sgtLogDebug(LogLevel::VERBOSE) << endl;
        for (const std::unique_ptr<WoNode>& nd1 : nodes())
        {
            sgtLogDebug(LogLevel::VERBOSE) << setw(5) << left << nd1->index();
            for (const std::unique_ptr<WoNode>& nd2 : nodes())
            {
                sgtLogDebug(LogLevel::VERBOSE) << setw(3) << left << (*nd1 < *nd2);
            }
            sgtLogDebug(LogLevel::VERBOSE) << endl;
        }
    }
}
