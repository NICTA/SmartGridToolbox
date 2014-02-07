#ifndef YY_TRANSFORMER_DOT_H
#define YY_TRANSFORMER_DOT_H

#include <SmartGridToolbox/Branch.h>

namespace SmartGridToolbox
{
   class YyTransformer : public Branch
   {
      public:
         YyTransformer(const std::string & name, Phases phases0, Phases phases1,
                       Complex a, Complex ZL, Complex YM);
      
      private:
         void recalcY();

      private:
         Complex a_; ///< Complex turns ratio, n0/n1.
         Complex YL_;    ///< Series leakage impedance.
         Complex YM_;    ///< Shunt magnetising impedance.
   };
}

#endif // YY_TRANSFORMER_DOT_H
