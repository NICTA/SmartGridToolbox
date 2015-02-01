#ifndef YY_TRANSFORMER_DOT_H
#define YY_TRANSFORMER_DOT_H

#include <SgtCore/Branch.h>

namespace SmartGridToolbox
{
   /// @brief Wye-wye transformer.
   ///
   /// Equivalent to a single phase transformer on each phase.
   class YyTransformer : public BranchAbc
   {
      public:

         SGT_PROPS_INIT(YyTransformer);
         SGT_PROPS_INHERIT(YyTransformer, Component);
         SGT_PROPS_INHERIT(YyTransformer, BranchAbc);

      /// @name Static member functions:
      /// @{
         
         static const std::string& sComponentType()
         {
            static std::string result("yy_transformer");
            return result;
         }
      
      /// @}

      /// @name Lifecycle
      /// @{
      
         /// @brief Constructor
         /// @param a The complex turns ratio (not voltage ratio) for each of the six windings.
         /// @param ZL The leakage impedance, must be > 0.
         YyTransformer(const std::string& id, Complex a, Complex ZL, Complex YM) :
            BranchAbc(id, Phase::A | Phase::B | Phase::C, Phase::A | Phase::B | Phase::C),
            a_(a),
            YL_(1.0 / ZL),
            YM_(YM)
         {
            // Empty.
         }

      /// @}

      /// @name Overridden from BranchAbc:
      /// @{

         virtual arma::Mat<Complex> inServiceY() const override;

      /// @}
      
      /// @name ComponentInterface virtual overridden functions.
      /// @{
        
         virtual const std::string& componentType() const override
         {
            return sComponentType();
         }

         // virtual void print(std::ostream& os) const override; // TODO

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

      private:
         Complex a_;  ///< Complex turns ratio, n0/n1.
         Complex YL_; ///< Series leakage admittance.
         Complex YM_; ///< Shunt magnetising impedance.
   };
}

#endif // YY_TRANSFORMER_DOT_H
