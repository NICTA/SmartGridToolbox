#ifndef SINGLE_PHASE_TRANSFORMER_DOT_H
#define SINGLE_PHASE_TRANSFORMER_DOT_H

#include <SgtCore/Branch.h>

namespace Sgt
{
   /// @brief Single phase transformer.
   class SinglePhaseTransformer : public BranchAbc
   {
      public:

      /// @name Static member functions:
      /// @{
         
         static const std::string& sComponentType()
         {
            static std::string result("single_phase_transformer");
            return result;
         }
      
      /// @}

         SGT_PROPS_INIT(SinglePhaseTransformer);
         SGT_PROPS_INHERIT(SinglePhaseTransformer, Component);
         SGT_PROPS_INHERIT(SinglePhaseTransformer, BranchAbc);

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

      /// @name ComponentInterface virtual overridden functions.
      /// @{
         
         virtual const std::string& componentType() const override
         {
            return sComponentType();
         }
         
         // virtual void print(std::ostream& os) const override; // TODO

      /// @}

      /// @name Overridden from BranchAbc:
      /// @{

         virtual arma::Mat<Complex> inServiceY() const override;

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

      private:

         Complex nomVRatio_; ///< Nominal voltage ratio.
         Complex offNomRatio_; ///< Off nominal complex turns ratio.
         Complex YL_; ///< Series leakage admittance.
   };
}

#endif // SINGLE_PHASE_TRANSFORMER_DOT_H
