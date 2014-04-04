#include "SinglePhaseTransformer.h"

namespace SmartGridToolbox
{
   SinglePhaseTransformer::SinglePhaseTransformer(const std::string & name, Phase phase0, Phase phase1,
                                                  Complex alpha, Complex ZLeak)
      : Branch(name, phase0, phase1),
        alpha_(alpha),
        ZLeak_(ZLeak)
   {
      recalcY();
   }

   void SinglePhaseTransformer::recalcY()
   {
      Complex YLeak = 1.0/ZLeak_;
      ublas::matrix<Complex> YNode(2, 2, czero);
      YNode(0, 0) = YLeak/(alpha_*conj(alpha_));
      YNode(0, 1) = -YLeak/conj(alpha_);
      YNode(1, 0) = -YLeak/alpha_;
      YNode(1, 1) = YLeak;
      setY(YNode);
   }
};
