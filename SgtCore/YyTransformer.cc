#include "YyTransformer.h"

namespace Sgt
{
    arma::Mat<Complex> YyTransformer::inServiceY() const
    {
        if (!isValid_)
        {
            validate();
        }
        return Y_;
    }

    void YyTransformer::validate() const
    {
        int n = phases0().size();
        Y_ = arma::Mat<Complex>(2 * n, 2 * n, arma::fill::zeros);
        for (int i = 0; i < n; ++i)
        {
            Y_(i, i) = (YL_ + YM_) / (a_ * conj(a_));
            Y_(i, i + n) = -YL_ / conj(a_);
            Y_(i + n, i) = -YL_ / a_;
            Y_(i + n, i + n) = YL_;
        }
    }
};
