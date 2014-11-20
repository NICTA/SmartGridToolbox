#include "YyTransformer.h"

namespace SmartGridToolbox
{
   const arma::Mat<Complex> YyTransformer::Y() const
   {
      int n = phases0().size();
      arma::Mat<Complex> Y(2 * n, 2 * n, czero);
      for (int i = 0; i < n; ++i)
      {
         Y(i, i) = (YL_ + YM_) / (a_ * conj(a_));
         Y(i, i + n) = -YL_ / conj(a_);
         Y(i + n, i) = -YL_ / a_;
         Y(i + n, i + n) = YL_;
      }
      return Y;
   }
};
