#include "YYTransformer.h"

namespace SmartGridToolbox
{
   YYTransformer::YYTransformer(const std::string & name, Phases phases0, Phases phases1,
                                ublas::vector<Complex> a, ublas::vector<Complex> ZL, ublas::vector<Complex> ZM)
      : Branch(name, phases0, phases1),
        a_(a)
   {
      int n = phases0.size();
      if (phases1.size() != n)
      {
         error() << "A Y_Y_transformer should have the same number of phases on the primary and secondary." 
                 << std::endl;
         abort();
      }
      if (a.size() != n)
      {
         error() << "A Y_Y_transformer should have the same number of turns ratios as there are phases."
                 << std::endl;
         abort();
      }
      if (ZL.size() != n)
      {
         error() << "A Y_Y_transformer should have the same number of leakage impedances as there are phases." 
                 << std::endl;
         abort();
      }
      if (ZM.size() != n)
      {
         error() << "A Y_Y_transformer should have the same number of magnetising impedances as there are phases." 
                 << std::endl;
         abort();
      }
      for (int i = 1; i < n; ++i)
      {
         YL_(i) = 1.0/ZL(i);
         YM_(i) = 1.0/ZM(i);
      }
      recalcY();      
   }

   void YYTransformer::recalcY()
   {
      int n = phases0().size();
      ublas::matrix<Complex> YNode(2*n, 2*n, czero);
      for (int i = 0; i < n; ++i)
      {
         YNode(i, i) = (YL_(i) + YM_(i)) /(a_(i)*conj(a_(i)));
         YNode(i, i + n) = -YL_(i)/conj(a_(i));  
         YNode(i + n, i) = -YL_(i)/a_(i);  
         YNode(i + n, i + n) = YL_(i);
      }
      setY(YNode);
   }
};
