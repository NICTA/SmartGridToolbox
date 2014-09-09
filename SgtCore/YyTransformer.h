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

      /// @name Lifecycle
      /// @{
      
         /// @brief Constructor
         /// @param a The complex turns ratio (not voltage ratio) for each of the six windings.
         /// @param ZL The leakage impedance, must be > 0.
         YyTransformer(const std::string& id, Complex a, Complex ZL, Complex ZM) :
            BranchAbc(id, Phase::A | Phase::B | Phase::C, Phase::A | Phase::B | Phase::C),
            a_(a),
            YL_(1.0 / ZL),
            YM_(1.0 / ZM)
         {
            // Empty.
         }

      /// @}
      
      /// @name Component Type:
      /// @{

         virtual const char* componentTypeStr() const {return "YY_transformer";}

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

         Complex ZM()
         {
            return 1.0 / YM_;
         }

         void setZM(Complex ZM)
         {
            YM_ = 1.0 / ZM;
         }

      /// @}

      /// @name Overridden from BranchAbc:
      /// @{

         virtual const ublas::matrix<Complex> Y() const override;

      /// @}
      
      private:
         void recalcY();

      /// @}
      
      private:
         Complex a_;  ///< Complex turns ratio, n0/n1.
         Complex YL_; ///< Series leakage admittance.
         Complex YM_; ///< Shunt magnetising impedance.
   };
}

#endif // YY_TRANSFORMER_DOT_H
