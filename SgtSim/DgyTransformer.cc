#include "DgyTransformer.h"

namespace SmartGridToolbox
{
   DgyTransformer::DgyTransformer(const std::string& name, Complex a, Complex ZL)
      : Branch(name, Phase::A | Phase::B | Phase::C, Phase::A | Phase::B | Phase::C), a_(a), YL_(1.0/ZL)
   {
      recalcY();
   }

   void DgyTransformer::recalcY()
   {
      Complex ai = 1.0/a_;
      Complex aci = 1.0/conj(a_);
      Complex a2i = ai*aci;

      Complex data[] =  {2*a2i,  -a2i,  -a2i, -aci,  0.0,  aci,
                          -a2i, 2*a2i,  -a2i,  aci, -aci,  0.0,
                          -a2i,  -a2i, 2*a2i,  0.0,  aci, -aci,
                           -ai,    ai,   0.0,  1.0,  0.0,  0.0,
                           0.0,   -ai,    ai,  0.0,  1.0,  0.0,
                            ai,   0.0,   -ai,  0.0,  0.0,  1.0};

      ublas::matrix<Complex> YNode(6, 6, czero);
      for (int i = 0; i < 6; ++i)
      {
         for (int j = 0; j < 6; ++j)
         {
            YNode(i, j) = YL_*data[6*i + j];
         }
      }
      setY(YNode);
   }
};
