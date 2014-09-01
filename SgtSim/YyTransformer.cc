#include "YyTransformer.h"

namespace SmartGridToolbox
{
   YyTransformer::YyTransformer(const std::string& id, Phases phases0, Phases phases1,
                                Complex a, Complex ZL, Complex YM)
      : SimBranch(id, phases0, phases1), a_(a), YL_(1.0 / ZL), YM_(YM)
   {
      if (phases0.size() != phases1.size())
      {
         error() << "A Y_Y_transformer should have the same number of phases on the primary and secondary."
                 << std::endl;
         abort();
      }
      recalcY();
   }

   void YyTransformer::recalcY()
   {
      int n = phases0().size();
      ublas::matrix<Complex> YNode(2 * n, 2 * n, czero);
      for (int i = 0; i < n; ++i)
      {
         YNode(i, i) = (YL_ + YM_) / (a_ * conj(a_));
         YNode(i, i + n) = -YL_ / conj(a_);
         YNode(i + n, i) = -YL_ / a_;
         YNode(i + n, i + n) = YL_;
      }
      setY(YNode);
   }
};
