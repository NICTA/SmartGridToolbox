#ifndef SINGLE_PHASE_TRANSFORMER_DOT_H
#define SINGLE_PHASE_TRANSFORMER_DOT_H

#include <SgtCore/Branch.h>

namespace SmartGridToolbox
{
   /// @brief Single phase transformer.
   class SinglePhaseTransformer : public BranchAbc
   {
      public:

      /// @name Lifecycle
      /// @{

         /// @brief Constructor
         /// @param a The complex turns ratio (not voltage ratio) for each of the six windings.
         /// @param ZL The leakage impedance, must be > 0.
         SinglePhaseTransformer(const std::string& id, Complex a, Complex ZL) :
            BranchAbc(id, Phase::BAL, Phase::BAL), a_(a), YL_(1.0/ZL)
         {
            // Empty.
         }

      /// @}

      /// @name Component Type:
      /// @{

         static constexpr const char* sComponentType()
         {
            return "single_phase_transformer";
         }

         virtual const char* componentType() const override
         {
            return sComponentType();
         }

      /// @}

      /// @name Parameters:
      /// @{

         Complex a()
         {
            return a_;
         }

         void set_a(Complex a)
         {
            a_ = a;
         }

         Complex YL()
         {
            return YL_;
         }

         void set_YL(Complex YL)
         {
            YL_ = YL;
         }

      /// @}

      /// @name Overridden from BranchAbc:
      /// @{

         virtual const arma::Mat<Complex> Y() const override;

      /// @}

      private:

         Complex a_;  ///< Complex turns ratio, n0/n1 where 0 is primary and 1 is secondary.
         Complex YL_; ///< Series leakage admittance.
   };
}

#endif // SINGLE_PHASE_TRANSFORMER_DOT_H
