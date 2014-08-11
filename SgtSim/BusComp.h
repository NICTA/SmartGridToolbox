#ifndef BUS_COMP_DOT_H
#define BUS_COMP_DOT_H

#include <SgtSim/Component.h>
#include <SgtSim/Event.h>

#include <SgtCore/Bus.h>
#include <SgtCore/Common.h>
#include <SgtCore/PowerFlow.h>

#include <iostream>

namespace SmartGridToolbox
{
   class ZipBase;

   /// @brief A Bus component of a Network.
   /// @ingroup PowerFlowCore
   class BusComp : public Component, public Bus
   {
      friend class BusCompParser;

      /// @name Overridden member functions from Component.
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
         
         BusComp(const std::string& id, Phases phases, const ublas::vector<Complex> & VNom, double VBase);
      
      /// @}

      /// @name Base class setters trigger changed().
      /// @{
         
         void setType(const BusType type)
         {
            Bus::setType(type);
            changed_.trigger();
         }
          
      /// @}
      
      /// @name Events.
      /// @{
         
         /// @brief Event triggered when bus (e.g. setpoint) has changed.
         Event& changed() {return changed_;}
      
      /// @}

      private:

      /// @name Events:
      /// @{
         
         Event changed_;

      /// @}
   };
}

#endif // BUS_COMP_DOT_H
