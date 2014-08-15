#ifndef BRANCH_COMP_DOT_H
#define BRANCH_COMP_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Common.h>
#include <SgtCore/PowerFlow.h>

#include <iostream>

namespace SmartGridToolbox
{
   class SimBus;

   /// @brief A SimBranch connects two Busses in a Network.
   /// @ingroup PowerFlowCore
   class SimBranch : public SimComponent
   {
      /// @name Public overridden member functions from SimComponent.
      /// @{
      
      public:
         // virtual Time validUntil() const override;

      protected:
         virtual void initializeState() override;
         virtual void updateState(Time t) override;
      
      /// @}

      public:
      
      /// @name Lifecycle:
      /// @{

         SimBranch(const std::string& name, const Phases& phases0, const Phases& phases1);
      
      /// @}

      /// @name SimBus accessors:
      /// @{
         
         const SimBus& bus0() const {return *bus0_;}
         void setBus0(SimBus& bus0);

         const SimBus& bus1() const {return *bus1_;}
         void setBus1(SimBus& bus1);
      
      /// @}

      /// @name Phase accessors:
      /// @{
         
         const Phases& phases0() const {return phases0_;}
         const Phases& phases1() const {return phases1_;}
      
      /// @}

      /// @name Nodal admittance matrix (Y) accessors:
      /// @{
         
         const ublas::matrix<Complex>& Y() const {return Y_;}
         void setY(const ublas::matrix<Complex>& Y);
      
      /// @}

      /// @name Custom events:
      /// @{

         /// @brief Event triggered whenever branch is changed in some way:
         Event& changed() {return changed_;}
      
      /// @}

      private:
         SimBus* bus0_;                  ///< My bus 0.
         SimBus* bus1_;                  ///< My bus 1.
         Phases phases0_;                 ///< Phases on bus 0.
         Phases phases1_;                 ///< Phases on bus 1.
         ublas::matrix<Complex> Y_;       ///< Complex value of elements in bus admittance matrix in NR solver.
         Event changed_;                  ///< Updates when a setpoint property of the branch changes.
   };
}

#endif // BRANCH_COMP_DOT_H
