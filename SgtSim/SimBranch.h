#ifndef BRANCH_COMP_DOT_H
#define BRANCH_COMP_DOT_H

#include <SgtSim/SimBus.h>
#include <SgtSim/SimObject.h>

#include <SgtCore/CommonBranch.h>
#include <SgtCore/PowerFlow.h>

#include <iostream>

namespace SmartGridToolbox
{
/// @brief A SimBranch connects two Busses in a Network.
/// @ingroup PowerFlowCore
class SimBranch : public CommonBranch, public SimObject
   {
      /// @name Public overridden member functions from SimObject.
      /// @{
      
      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;
      
      /// @}

      public:
      
      /// @name Lifecycle:
      /// @{

         SimBranch(const std::string& name, const Phases& phases0, const Phases& phases1);
      
      /// @}

      /// @name SimBus accessors:
      /// @{
         
         const SimBus& bus0() const {return dynamic_cast<const SimBus&>(CommonBranch::bus0());}
         const SimBus& bus1() const {return dynamic_cast<const SimBus&>(Branch::bus1());}
      
      /// @}
      
      /// @name Bus accessors:
      /// @{

         virtual void setBus0(const std::shared_ptr<Bus>& bus0)
         {
            Branch::setBus0(bus0);
            changed_.trigger();
         }
          
         virtual void setBus1(const std::shared_ptr<Bus>& bus1)
         {
            Branch::setBus1(bus1);
            changed_.trigger();
         }

      /// @name Custom events:
      /// @{

         /// @brief Event triggered whenever branch is changed in some way:
         Event& changed() {return changed_;}
      
      /// @}

      private:
         Event changed_;                  ///< Updates when a setpoint property of the branch changes.
   };
}

#endif // BRANCH_COMP_DOT_H
