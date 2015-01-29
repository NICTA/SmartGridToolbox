#ifndef DGY_TRANSFORMER_DOT_H
#define DGY_TRANSFORMER_DOT_H

#include <SgtCore/Branch.h>

namespace SmartGridToolbox
{
   /// @brief Delta-grounded wye transformer.
   class DgyTransformer : public Component, public BranchAbc
   {
      public:

         SGT_PROPS_INIT(DgyTransformer);
         SGT_PROPS_INHERIT(DgyTransformer, Component);
         SGT_PROPS_INHERIT(DgyTransformer, BranchAbc);

      /// @name Lifecycle
      /// @{

         /// @brief Constructor
         /// @param a The complex turns ratio (not voltage ratio) for each of the six windings.
         /// @param ZL The leakage impedance, must be > 0.
         DgyTransformer(const std::string& id, Complex nomVRatioDY, Complex offNomRatioDY, Complex ZL) :
            Component(id), BranchAbc(Phase::A | Phase::B | Phase::C, Phase::A | Phase::B | Phase::C),
            nomVRatioDY_(nomVRatioDY), offNomRatioDY_(offNomRatioDY), YL_(1.0/ZL)
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

         Complex nomVRatioDY() const
         {
            return nomVRatioDY_;
         }
         
         void setNomVRatioDY(Complex nomVRatioDY)
         {
            nomVRatioDY_ = nomVRatioDY;
         }

         Complex offNomRatioDY() const
         {
            return offNomRatioDY_;
         }
 
         void setOffNomRatioDY(Complex offNomRatioDY)
         {
            offNomRatioDY_ = offNomRatioDY;
         }
 
         Complex a() const
         {
            return offNomRatioDY_ * nomVRatioDY_;  
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
         
      protected:

      /// @name Printing:
      /// @{
         
         virtual void print(std::ostream& os) const
         {
            // TODO: proper printing.
            BranchAbc::print(os);
         }

      /// @}

      private:

         Complex nomVRatioDY_; ///< Nominal voltage ratio, V_D / V_Y where V_D is phase-phase and V_Y is phase-ground.
         Complex offNomRatioDY_; ///< Off nominal complex turns ratio.
         Complex YL_; ///< Series leakage admittance.
   };
}

#endif // DGY_TRANSFORMER_DOT_H
