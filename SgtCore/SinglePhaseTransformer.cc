#include "SinglePhaseTransformer.h"

namespace Sgt
{
   arma::Mat<Complex> SinglePhaseTransformer::inServiceY() const
   {
      Complex ai = 1.0 / a();
      Complex aci = conj(ai);
      Complex a2i = ai * aci;
      arma::Mat<Complex> Y(2, 2, arma::fill::zeros);
      Y(0, 0) = YL_ * a2i;
      Y(0, 1) = -YL_ * aci;
      Y(1, 0) = -YL_ * ai;
      Y(1, 1) = YL_;
      return Y;
   }
};
