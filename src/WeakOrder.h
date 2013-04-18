#include <vector>
#include <list>
#include <set>

namespace SmartGridToolbox
{
   class WONode
   {
      friend class WOGraph;

      public:
         WONode(int idx) : idx_(idx), 
                           visited_(false), 
                           finished_(false), 
                           just_finished_(false),
                           in_stack_(false)
         {
            // Empty.
         }

         void DFS(std::vector<WONode *> & stack);

         int Index() const
         {
            return idx_;
         }

         bool Precedes(const WONode & nd) const
         {
            std::set<const WONode *>::iterator it = descendents_.find(&nd);
            return (it != descendents_.end());
         }

         bool Dominates(const WONode & nd) const
         {
            return (dominated_.find(&nd) != dominated_.end());
         }
         
         friend bool operator<(const WONode & lhs, 
                               const WONode & rhs)
         {
            return (rhs.Dominates(lhs) || 
                    ((!lhs.Dominates(rhs)) && (lhs.idx_ < rhs.idx_)));
         }

      private:

         int idx_;
         bool visited_;
         bool finished_;
         bool just_finished_;
         bool in_stack_;
         std::list<WONode *> to_;
         std::set<const WONode *> descendents_;
         std::set<const WONode *> dominated_;
   };

   class WOGraph
   {
      public:
         WOGraph(int n);

         ~WOGraph();

         void Link(int from, int to)
         {
            nodes_[from]->to_.push_back(nodes_[to]);
         }

         void WeakOrder();

         std::vector<WONode *>::const_iterator begin() const
         {
            return nodes_.begin();
         }

         std::vector<WONode *>::const_iterator end() const
         {
            return nodes_.end();
         }

         int Size() const
         {
            return nodes_.size();
         }

         const WONode & operator[](int idx) const
         {
            return *nodes_[idx];
         }
         WONode & operator[](int idx)
         {
            return *nodes_[idx];
         }

      private:
         int n_;
         std::vector<WONode *> nodes_;
   };
}
