#ifndef YY_TRANSFORMER_DOT_H
#define YY_TRANSFORMER_DOT_H

#include <SgtSim/SimBranch.h>

namespace SmartGridToolbox
{
   /// @brief Wye-wye transformer.
   ///
   /// Equivalent to a single phase transformer on each phase.
   class YyTransformer : public SimBranch
   {
      /// @name Overridden member functions from SimComponent.
      /// @{
      
      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;
      
      /// @}

      /// @name My member functions.
      /// @{
      
      public:
         YyTransformer(const std::string& name, Phases phases0, Phases phases1,
                       Complex a, Complex ZL, Complex YM);

      private:
         void recalcY();

      /// @}
      
      private:
         Complex a_;  ///< Complex turns ratio, n0/n1.
         Complex YL_; ///< Series leakage impedance.
         Complex YM_; ///< Shunt magnetising impedance.
   };
}

#endif // YY_TRANSFORMER_DOT_H
