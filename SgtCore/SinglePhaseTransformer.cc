#include "SinglePhaseTransformer.h"

namespace SmartGridToolbox
{
   const arma::Mat<Complex> SinglePhaseTransformer::Y() const
   {
      arma::Mat<Complex> Y(2, 2, arma::fill::zeros);
      Y(0, 0) = YL_ / (a_ * conj(a_));
      Y(0, 1) = -YL_ / conj(a_);
      Y(1, 0) = -YL_ / a_;
      Y(1, 1) = YL_;
      return Y;
   }
};
