// Copyright 2015 National ICT Australia Limited (NICTA)
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

#ifndef WEAK_ORDER_DOT_H
#define WEAK_ORDER_DOT_H

#include <vector>
#include <list>
#include <memory>
#include <set>

namespace Sgt
{
    class WoNode
    {
        friend class WoGraph;

        public:
            WoNode(std::size_t idx) : idx_(idx), visited_(false)
            {
                // Empty.
            }

            std::size_t index() const
            {
                return idx_;
            }

            /// @brief Is there a link from here to nd?
            ///
            /// This relation could model dependency: nd would directly depend on me.
            bool precedes(const WoNode& nd) const
            {
                std::set<const WoNode*>::iterator it = descendents_.find(&nd);
                return (it != descendents_.end());
            }

            /// @brief Can I get from here to nd, but not the reverse?
            ///
            /// This relation could model dependency: nd would indirectly depend on me, and not vice-versa.
            /// Thus domination == "goes first."
            bool dominates(const WoNode& nd) const
            {
                return (dominated_.find(&nd) != dominated_.end());
            }

            /// @brief Partial ordering operator.
            ///
            /// Smaller goes first. So lhs < rhs if lhs dominates rhs.
            friend bool operator<(const WoNode& lhs, const WoNode& rhs)
            {
                return (lhs.dominates(rhs) || ((!rhs.dominates(lhs)) && (lhs.idx_ < rhs.idx_)));
            }

            /// @brief Perform depth first search.
            void dfs(std::vector<WoNode*>& stack);

        private:

            std::size_t idx_;
            bool visited_;
            std::list<WoNode*> to_;
            std::set<const WoNode*> descendents_; // Can be reached from here.
            std::set<const WoNode*> dominated_; // Can be reached from here, not vice-versa.
    };

    class WoGraph
    {
        public:
            WoGraph(std::size_t n);

            void link(std::size_t from, std::size_t to)
            {
                nodes_[from]->to_.push_back(nodes_[to].get());
            }

            void weakOrder();

            const std::vector<std::unique_ptr<WoNode>>& nodes() const
            {
                return nodes_;
            }
            std::vector<std::unique_ptr<WoNode>>& nodes()
            {
                return nodes_;
            }

            std::size_t size() const
            {
                return nodes_.size();
            }

        private:
            void debugPrint();

        private:
            std::vector<std::unique_ptr<WoNode>> nodes_;
    };
}

#endif // WEAK_ORDER_DOT_H
