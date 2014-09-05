#include "SinglePhaseTransformer.h"

namespace SmartGridToolbox
{
   const ublas::matrix<Complex> SinglePhaseTransformer::Y() const
   {
      ublas::matrix<Complex> Y(2, 2, czero);
      Y(0, 0) = YL_ / (a_ * conj(a_));
      Y(0, 1) = -YL_ / conj(a_);
      Y(1, 0) = -YL_ / a_;
      Y(1, 1) = YL_;
      return Y;
   }
};
