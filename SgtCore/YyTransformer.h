#ifndef YY_TRANSFORMER_DOT_H
#define YY_TRANSFORMER_DOT_H

#include <SgtCore/Branch.h>

namespace SmartGridToolbox
{
   /// @brief Wye-wye transformer.
   ///
   /// Equivalent to a single phase transformer on each phase.
   class YyTransformer : public Component, public BranchAbc
   {
      public:

         SGT_PROPS_INIT(YyTransformer);
         SGT_PROPS_INHERIT(YyTransformer, Component);
         SGT_PROPS_INHERIT(YyTransformer, BranchAbc);

      /// @name Lifecycle
      /// @{
      
         /// @brief Constructor
         /// @param a The complex turns ratio (not voltage ratio) for each of the six windings.
         /// @param ZL The leakage impedance, must be > 0.
         YyTransformer(const std::string& id, Complex a, Complex ZL, Complex YM) :
            Component(id),
            BranchAbc(Phase::A | Phase::B | Phase::C, Phase::A | Phase::B | Phase::C),
            a_(a),
            YL_(1.0 / ZL),
            YM_(YM)
         {
            // Empty.
         }

      /// @}
      
      /// @name Component Type:
      /// @{

         static constexpr const char* sComponentType()
         {
            return "yy_transformer";
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

         Complex YM()
         {
            return YM_;
         }

         void setYM(Complex YM)
         {
            YM_ = YM;
         }

      /// @}

      /// @name Overridden from BranchAbc:
      /// @{

         virtual arma::Mat<Complex> inServiceY() const override;

      /// @}

      /// @name Printing:
      /// @{
         
         virtual void print(std::ostream& os) const
         {
            // TODO: proper printing.
            BranchAbc::print(os);
         }

      /// @}
      
      private:
         Complex a_;  ///< Complex turns ratio, n0/n1.
         Complex YL_; ///< Series leakage admittance.
         Complex YM_; ///< Shunt magnetising impedance.
   };
}

#endif // YY_TRANSFORMER_DOT_H
