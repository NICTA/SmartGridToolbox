#ifndef BUS_COMP_DOT_H
#define BUS_COMP_DOT_H

#include <SgtSim/SimObject.h>

#include <SgtCore/Bus.h>
#include <SgtCore/PowerFlow.h>

#include <iostream>

namespace SmartGridToolbox
{
   class SimZip;
   class SimGen;

   /// @brief A Bus component of a Network.
   /// @ingroup PowerFlowCore
   class SimBus : public Bus, public SimObject
   {
      friend class SimBusParser;

      /// @name Overridden member functions from SimObject.
      /// @{

      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;
      
      /// @}
      
      public:
      /// @name Lifecycle.
      /// @{
         
         SimBus(const std::string& id, Phases phases, const ublas::vector<Complex> & VNom, double VBase) :
            Bus(id, phases, VNom, VBase),
            changed_("SimBus " + id + " setpoint changed")
         {
            // Empty.
         }

      /// @}

      private:

      /// @name PowerFlowBus:
      /// @{

      /// @}
   };
}

#endif // BUS_COMP_DOT_H
