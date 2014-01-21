#ifndef Y_Y_TRANSFORMER_DOT_H
#define Y_Y_TRANSFORMER_DOT_H

#include <SmartGridToolbox/Branch.h>

namespace SmartGridToolbox
{
   class YYTransformer : public Branch
   {
      public:
         YYTransformer(const std::string & name, Phases phases0, Phases phases1,
                       ublas::vector<Complex> alpha, ublas::vector<Complex> ZL, ublas::vector<Complex> ZM);
      
      private:
         void recalcY();

      private:
         ublas::vector<Complex> alpha_; ///< Complex turns ratio, n0/n1.
         ublas::vector<Complex> YL_; ///< Series leakage impedance.
         ublas::vector<Complex> YM_; ///< Shunt magnetising impedance.
   };
}

#endif // Y_Y_TRANSFORMER_DOT_H
