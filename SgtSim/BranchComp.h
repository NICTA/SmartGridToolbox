#ifndef BRANCH_DOT_H
#define BRANCH_DOT_H

#include <SgtSim/Component.h>

#include <SgtCore/Common.h>
#include <SgtCore/PowerFlow.h>

#include <iostream>

namespace SmartGridToolbox
{
   class BusComp;

   /// @brief A BranchComp connects two Busses in a Network.
   /// @ingroup PowerFlowCore
   class BranchComp : public Component
   {
      /// @name Public overridden member functions from Component.
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

         BranchComp(const std::string& name, const Phases& phases0, const Phases& phases1);
      
      /// @}

      /// @name BusComp accessors:
      /// @{
         
         const BusComp& bus0() const {return *bus0_;}
         void setBus0(BusComp& bus0);

         const BusComp& bus1() const {return *bus1_;}
         void setBus1(BusComp& bus1);
      
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
         BusComp* bus0_;                  ///< My bus 0.
         BusComp* bus1_;                  ///< My bus 1.
         Phases phases0_;                 ///< Phases on bus 0.
         Phases phases1_;                 ///< Phases on bus 1.
         ublas::matrix<Complex> Y_;       ///< Complex value of elements in bus admittance matrix in NR solver.
         Event changed_;                  ///< Updates when a setpoint property of the branch changes.
   };
}

#endif // BRANCH_DOT_H
