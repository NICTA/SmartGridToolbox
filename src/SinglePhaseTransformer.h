#ifndef SINGLE_PHASE_TRANSFORMER_DOT_H
#define SINGLE_PHASE_TRANSFORMER_DOT_H

#include <SmartGridToolbox/Branch.h>

namespace SmartGridToolbox
{
   class SinglePhaseTransformer : public Branch
   {
      public:
         SinglePhaseTransformer(const std::string & name, Complex alpha, Complex ZLeak);
      
      private:
         Complex alpha01; ///< Complex turns ratio, n0/n1.
         Complex ZLeak;   ///< Leakage impedance.
   };
}

#endif // SINGLE_PHASE_TRANSFORMER_DOT_H
