#include "YYTransformer.h"

namespace SmartGridToolbox
{
   YYTransformer::YYTransformer(const std::string & name, Phases phases0, Phases phases1,
                                ublas::vector<Complex> alpha, ublas::vector<Complex> ZLeak)
      : Branch(name, phases0, phases1),
        alpha_(alpha),
        ZLeak_(ZLeak)
   {
      int n = phases0.size();
      if (phases1.size() != n)
      {
         error() << "A Y_Y_transformer should have the same number of phases on the primary and secondary." 
                 << std::endl;
         abort();
      }
      if (alpha.size() != n)
      {
         error() << "A Y_Y_transformer should have the same number of turns ratios as there are phases."
                 << std::endl;
         abort();
      }
      if (ZLeak.size() != n)
      {
         error() << "A Y_Y_transformer should have the same number of leakage impedances as there are phases." 
                 << std::endl;
         abort();
      }
      recalcY();      
   }

   void YYTransformer::recalcY()
   {
      int n = phases0().size();
      ublas::matrix<Complex> YNode(2 * n, 2 * n, czero);
      for (int i = 0; i < n; ++i)
      {
         Complex YLeak = 1.0 / ZLeak_(i);
         YNode(i, i) = YLeak / (alpha_(i) * conj(alpha_(i)));
         YNode(i, i + n) = -YLeak / conj(alpha_(i));  
         YNode(i + n, i) = -YLeak / alpha_(i);  
         YNode(i + n, i + n) = YLeak;
      }
      setY(YNode);
   }
};
