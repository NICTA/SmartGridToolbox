#include "DgyTransformer.h"

namespace Sgt
{
   arma::Mat<Complex> DgyTransformer::inServiceY() const
   {
      if (!isValid_)
      {
         validate();
      }
      return Y_;
   }

   void DgyTransformer::validate() const
   {
      Complex ai = 1.0 / a();
      Complex aci = conj(ai);
      Complex a2i = ai * aci;

      Complex data[] =  {2 * a2i,  -a2i,  -a2i, -aci,  0.0,  aci,
                          -a2i, 2 * a2i,  -a2i,  aci, -aci,  0.0,
                          -a2i,  -a2i, 2 * a2i,  0.0,  aci, -aci,
                           -ai,    ai,   0.0,  1.0,  0.0,  0.0,
                           0.0,   -ai,    ai,  0.0,  1.0,  0.0,
                            ai,   0.0,   -ai,  0.0,  0.0,  1.0};

      Y_ = arma::Mat<Complex>(6, 6, arma::fill::none);
      for (int i = 0; i < 6; ++i)
      {
         for (int j = 0; j < 6; ++j)
         {
            Y_(i, j) = YL_ * data[6 * i + j];
         }
      }
   }
};
