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
         SinglePhaseTransformer(const std::string& id, Complex nomVRatio, Complex offNomRatio, Complex ZL) :
            BranchAbc(id, Phase::BAL, Phase::BAL), nomVRatio_(nomVRatio), offNomRatio_(offNomRatio), YL_(1.0/ZL)
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

         Complex nomVRatio() const
         {
            return nomVRatio_;
         }
         
         void setNomVRatio(Complex nomVRatio)
         {
            nomVRatio_ = nomVRatio;
         }

         Complex offNomRatio() const
         {
            return offNomRatio_;
         }
 
         void setOffNomRatio(Complex offNomRatio)
         {
            offNomRatio_ = offNomRatio;
         }
 
         Complex a() const
         {
            return offNomRatio_ * nomVRatio_;  
         }

         Complex ZL() const
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

         virtual arma::Mat<Complex> inServiceY() const override;

      /// @}

      private:

         Complex nomVRatio_; ///< Nominal voltage ratio.
         Complex offNomRatio_; ///< Off nominal complex turns ratio.
         Complex YL_; ///< Series leakage admittance.
   };
}

#endif // SINGLE_PHASE_TRANSFORMER_DOT_H
