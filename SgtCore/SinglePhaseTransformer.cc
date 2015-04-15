#include "SinglePhaseTransformer.h"

namespace Sgt
{
    arma::Mat<Complex> SinglePhaseTransformer::inServiceY() const
    {
        if (!isValid_)
        {
            validate();
        }
        return Y_;
    }

    void SinglePhaseTransformer::validate() const
    {
        Complex ai = 1.0 / a();
        Complex aci = conj(ai);
        Complex a2i = ai * aci;
        Y_ = arma::Mat<Complex>(2, 2, arma::fill::none);
        Y_(0, 0) = YL_ * a2i;
        Y_(0, 1) = -YL_ * aci;
        Y_(1, 0) = -YL_ * ai;
        Y_(1, 1) = YL_;
    }
};
