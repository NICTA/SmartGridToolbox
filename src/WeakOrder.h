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

         int getIndex() const
         {
            return idx_;
         }

         bool precedes(const WoNode & nd) const
         {
            std::set<const WoNode *>::iterator it = descendents_.find(&nd);
            return (it != descendents_.end());
         }

         bool dominates(const WoNode & nd) const
         {
            return (dominated_.find(&nd) != dominated_.end());
         }
         
         friend bool operator<(const WoNode & lhs, 
                               const WoNode & rhs)
         {
            return (rhs.dominates(lhs) || 
                    ((!lhs.dominates(rhs)) && (lhs.idx_ < rhs.idx_)));
         }

         void dfs(std::vector<WoNode *> & stack);

      private:

         int idx_;
         bool visited_;
         std::list<WoNode *> to_;
         std::set<const WoNode *> descendents_;
         std::set<const WoNode *> dominated_;
   };

   class WoGraph
   {
      public:
         WoGraph(int n);

         ~WoGraph();

         void link(int from, int to)
         {
            nodes_[from]->to_.push_back(nodes_[to]);
         }

         void weakOrder();

         const std::vector<WoNode *> & getNodes() const
         {
            return nodes_;
         }
         std::vector<WoNode *> & getNodes() 
         {
            return nodes_;
         }

         int size() const
         {
            return nodes_.size();
         }

         const WoNode & operator[](int idx) const
         {
            return *nodes_[idx];
         }
         WoNode & operator[](int idx)
         {
            return *nodes_[idx];
         }

      private:
         int n_;
         std::vector<WoNode *> nodes_;
   };
}
