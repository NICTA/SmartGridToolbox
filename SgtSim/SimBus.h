#ifndef BUS_COMP_DOT_H
#define BUS_COMP_DOT_H

#include <SgtSim/Event.h>
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
         
         SimBus(const std::string& id, Phases phases, const ublas::vector<Complex> & VNom, double VBase);

      /// @}

      /// @name Base class setters trigger changed().
      /// @{
         
         virtual void setType(const BusType type)
         {
            Bus::setType(type);
            changed_.trigger();
         }
          
         virtual void setVMagSetpoint(const ublas::vector<double>& VMagSetpoint)
         {
            Bus::setVMagSetpoint(VMagSetpoint);
            changed_.trigger();
         }

         virtual void setVAngSetpoint(const ublas::vector<double>& VAngSetpoint)
         {
            Bus::setVAngSetpoint(VAngSetpoint);
            changed_.trigger();
         }

         virtual void setVMagMin(double VMagMin)
         {
            Bus::setVMagMin(VMagMin);
            changed_.trigger();
         }
         
         virtual void setVMagMax(double VMagMax)
         {
            Bus::setVMagMax(VMagMax);
            changed_.trigger();
         }
         
         virtual void setIsInService(bool isInService)
         {
            Bus::setIsInService(isInService);
            changed_.trigger();
         }

         virtual void setV(const ublas::vector<Complex>& V)
         {
            Bus::setV(V);
            changed_.trigger();
         }

      /// @}
      
      /// @name Events.
      /// @{
         
         /// @brief Event triggered when bus (e.g. setpoint) has changed.
         virtual Event& changed() {return changed_;}
      
      /// @}

      private:

      /// @name PowerFlowBus:
      /// @{

      /// @}
      
      /// @name Events:
      /// @{
         
         Event changed_;

      /// @}
   };
}

#endif // BUS_COMP_DOT_H
