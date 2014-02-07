#ifndef YY_TRANSFORMER_DOT_H
#define YY_TRANSFORMER_DOT_H

#include <SmartGridToolbox/Branch.h>

namespace SmartGridToolbox
{
   class YyTransformer : public Branch
   {
      public:
         YyTransformer(const std::string & name, Phases phases0, Phases phases1,
                       ublas::vector<Complex> a, ublas::vector<Complex> ZL, ublas::vector<Complex> ZM);
      
      private:
         void recalcY();

      private:
         ublas::vector<Complex> a_; ///< Complex turns ratio, n0/n1.
         ublas::vector<Complex> YL_;    ///< Series leakage impedance.
         ublas::vector<Complex> YM_;    ///< Shunt magnetising impedance.
   };
}

#endif // YY_TRANSFORMER_DOT_H
