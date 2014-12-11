#include "YyTransformer.h"

namespace SmartGridToolbox
{
   arma::Mat<Complex> YyTransformer::inServiceY() const
   {
      int n = phases0().size();
      arma::Mat<Complex> Y(2 * n, 2 * n, arma::fill::zeros);
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
