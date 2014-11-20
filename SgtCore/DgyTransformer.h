#ifndef DGY_TRANSFORMER_DOT_H
#define DGY_TRANSFORMER_DOT_H

#include <SgtCore/Branch.h>

namespace SmartGridToolbox
{
   /// @brief Delta-grounded wye transformer.
   ///
   /// Note that a 1:1 turns ratio will not result in a 1:1 voltage ratio for this transformer connection.
   /// This is because the voltage across the windings on the primary (Delta) side is the phase-phase voltage, so
   /// a 1:1 turns ratio will result in a @f$1:\sqrt(3)@f$ voltage ratio, where all voltages are line-ground. It is
   /// therefore important that the parameter to the constructor is the turns ratio, not the voltage ratio.
   class DgyTransformer : public BranchAbc
   {
      public:

      /// @name Lifecycle
      /// @{

         /// @brief Constructor
         /// @param a The complex turns ratio (not voltage ratio) for each of the six windings.
         /// @param ZL The leakage impedance, must be > 0.
         DgyTransformer(const std::string& id, Complex a, Complex ZL) :
            BranchAbc(id, Phase::A | Phase::B | Phase::C, Phase::A | Phase::B | Phase::C), a_(a), YL_(1.0/ZL)
         {
            // Empty.
         }

      /// @}

      /// @name Component Type:
      /// @{

         static constexpr const char* sComponentType()
         {
            return "dgy_transformer";
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

         Complex ZL()
         {
            return 1.0 / YL_;
         }

         void setZL(Complex ZL)
         {
            YL_ = 1.0 / ZL;
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

#endif // DGY_TRANSFORMER_DOT_H
