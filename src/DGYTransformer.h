#ifndef D_GY_TRANSFORMER_DOT_H
#define D_GY_TRANSFORMER_DOT_H

#include <SmartGridToolbox/Branch.h>

namespace SmartGridToolbox
{
   class DGYTransformer : public Branch
   {
      public:
         DGYTransformer(const std::string & name, Phases phases0, Phases phases1,
                       ublas::vector<Complex> alpha, ublas::vector<Complex> ZL, ublas::vector<Complex> ZM);
      
      private:
         void recalcY();

      private:
         ublas::vector<Complex> alpha_; ///< Complex turns ratio, n0/n1.
         ublas::vector<Complex> YL_;    ///< Series leakage impedance.
         ublas::vector<Complex> YM_;    ///< Shunt magnetising impedance.
   };
}

#endif // D_GY_TRANSFORMER_DOT_H
