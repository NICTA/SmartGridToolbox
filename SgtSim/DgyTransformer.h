#ifndef DGY_TRANSFORMER_DOT_H
#define DGY_TRANSFORMER_DOT_H

#include <SgtSim/Branch.h>

namespace SmartGridToolbox
{
   /// @brief Delta-grounded wye transformer.
   ///
   /// Note that a 1:1 turns ratio will not result in a 1:1 voltage ratio for this transformer connection.
   /// This is because the voltage across the windings on the primary (Delta) side is the phase-phase voltage, so
   /// a 1:1 turns ratio will result in a @f$1:\sqrt(3)@f$ voltage ratio, where all voltages are line-ground. It is
   /// therefore important that the parameter to the constructor is the turns ratio, not the voltage ratio.
   class DgyTransformer : public Branch
   {
      /// @name Overridden member functions from Component.
      /// @{
      
      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;

      /// @}
      
      public:
      /// @name My public member functions.
      /// @{
         
         /// @brief Constructor
         /// @param a The complex turns ratio (not voltage ratio) for each of the six windings.
         /// @param ZL The leakage impedance, must be > 0.
         DgyTransformer(const std::string& name, Complex a, Complex ZL);

         /// @}
      
      private:
         /// @name My private member functions.
         /// @{
         
         void recalcY();

      /// @}

      private:
         Complex a_;  ///< Complex turns ratio, n0/n1 where 0 is primary and 1 is secondary.
         Complex YL_; ///< Series leakage impedance.
   };
}

#endif // D_GY_TRANSFORMER_DOT_H
