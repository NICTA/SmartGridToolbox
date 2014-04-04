#include <vector>
#include <list>
#include <set>

namespace SmartGridToolbox
{
   class WoNode
   {
      friend class WoGraph;

      public:
         WoNode(int idx) : idx_(idx), visited_(false)
         {
            // Empty.
         }

         int index() const
         {
            return idx_;
         }

         /// @brief Is there a link from here to nd?
         ///
         /// This relation could model dependency: nd would directly depend on me.
         bool precedes(const WoNode & nd) const
         {
            std::set<const WoNode*>::iterator it = descendents_.find(&nd);
            return (it != descendents_.end());
         }

         /// @brief Can I get from here to nd, but not the reverse?
         ///
         /// This relation could model dependency: nd would indirectly depend on me, and not vice-versa.
         /// Thus domination == "goes first."
         bool dominates(const WoNode & nd) const
         {
            return (dominated_.find(&nd) != dominated_.end());
         }

         /// @brief Partial ordering operator.
         ///
         /// Smaller goes first. So lhs < rhs if lhs dominates rhs.
         friend bool operator<(const WoNode & lhs, const WoNode & rhs)
         {
            return (lhs.dominates(rhs) || ((!rhs.dominates(lhs)) && (lhs.idx_ < rhs.idx_)));
         }

         /// @brief Perform depth first search.
         void dfs(std::vector<WoNode*> & stack);

      private:

         int idx_;
         bool visited_;
         std::list<WoNode*> to_;
         std::set<const WoNode*> descendents_; // Can be reached from here.
         std::set<const WoNode*> dominated_; // Can be reached from here, not vice-versa.
   };

   class WoGraph
   {
      public:
         WoGraph(int n);

         void link(int from, int to)
         {
            nodes_[from]->to_.push_back(nodes_[to].get());
         }

         void weakOrder();

         const std::vector<std::unique_ptr<WoNode>> & nodes() const
         {
            return nodes_;
         }
         std::vector<std::unique_ptr<WoNode>> & nodes()
         {
            return nodes_;
         }

         int size() const
         {
            return nodes_.size();
         }

      private:
         void debugPrint();

      private:
         int n_;
         std::vector<std::unique_ptr<WoNode>> nodes_;
   };
}
