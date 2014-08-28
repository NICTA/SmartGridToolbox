#ifndef SIM_BUS_DOT_H
#define SIM_BUS_DOT_H

#include <SgtSim/SimObject.h>

#include <SgtCore/Bus.h>
#include <SgtCore/PowerFlow.h>

#include <iostream>

namespace SmartGridToolbox
{
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
            Bus(id, phases, VNom, VBase)
         {
            // Empty.
         }

      /// @}
   };
}

#endif // SIM_BUS_DOT_H
