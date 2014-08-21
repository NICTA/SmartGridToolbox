#ifndef SINGLE_PHASE_TRANSFORMER_DOT_H
#define SINGLE_PHASE_TRANSFORMER_DOT_H

#include <SgtSim/SimBranch.h>

namespace SmartGridToolbox
{
   /// @brief Single phase transformer.
   class SinglePhaseTransformer : public SimBranch
   {
      /// @name Overridden member functions from SimObject.
      /// @{
      
      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;
      
      /// @}

      /// @name My public member functions.
      /// @{
      
      public:
         SinglePhaseTransformer(const std::string& name, Phase phase0, Phase phase1, Complex alpha, Complex ZLeak);

      /// @}
      
      /// @name My private member functions.
      /// @{
      
      private:
         void recalcY();

      /// @}
      
      private:
         Complex alpha_; ///< Complex turns ratio, n0/n1.
         Complex ZLeak_; ///< Leakage impedance.
   };
}

#endif // SINGLE_PHASE_TRANSFORMER_DOT_H
