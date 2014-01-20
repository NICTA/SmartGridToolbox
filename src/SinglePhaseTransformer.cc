#include "SinglePhaseTransformer.h"

namespace SmartGridToolbox
{
   SinglePhaseTransformer::SinglePhaseTransformer(const std::string & name, Phase phase0, Phase phase1, &
                                                  Complex alpha, Complex ZLeak)
      : Branch(name, phase0, phase1),
        alpha_(alpha),
        ZLeak_(ZLeak)

   {
      recalcY();      
   }

   void SinglePhaseTransformer::recalcY()
   {
      double a2 = real(a * conj(a));
      ublas::matrix<Complex> YNode(2, 2, czero);
      YNode(0, 0) = YLeak_ / a2;  
      YNode(0, 1) = -YLeak_ / conj(a);  
      YNode(1, 0) = -YLeak_ / a;  
      YNode(1, 1) = YLeak_;
      setY(YNode);
   }
};
