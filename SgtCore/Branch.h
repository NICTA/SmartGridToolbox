#ifndef BRANCH_DOT_H
#define BRANCH_DOT_H

#include <SgtCore/Component.h>
#include <SgtCore/Event.h>
#include <SgtCore/PowerFlow.h>

#include <iostream>

namespace SmartGridToolbox
{
   /// @brief Branch is an abstract base class for a branch which connects two Busses in a Network.
   class Branch : public Component
   {
      public:

      /// @name Lifecycle:
      /// @{
         
         Branch(const std::string& id, Phases phases0, Phases phases1);

      /// @}

      /// @name Component Type:
      /// @{
         
         virtual const char* componentTypeStr() const {return "branch";}

      /// @}
 
      /// @name Phase accessors:
      /// @{
         
         const Phases& phases0() const
         {
            return phases0_;
         }
         
         virtual void setPhases0(Phases& phases0)
         {
            phases0_ = phases0;
         }
      
         const Phases& phases1() const
         {
            return phases1_;
         }
         
         virtual void setPhases1(Phases& phases1)
         {
            phases1_ = phases1;
         }
      
      /// @}
 
      /// @name In service:
      /// @{
         
         bool isInService() const
         {
            return isInService_;
         }

         virtual void setIsInService(bool isInService)
         {
            isInService_ = isInService;
            isInServiceChanged_.trigger();
         }

      /// @}

      /// @name Nodal admittance matrix (Y):
      /// @{
         
         virtual const ublas::matrix<Complex> Y() const = 0;
      
      /// @}
      
      /// @name Events.
      /// @{
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& isInServiceChanged() {return isInServiceChanged_;}

         /// @brief Event triggered when my admittance changes.
         virtual Event& admittanceChanged() {return admittanceChanged_;}
         
      /// @}
      
      protected:

         virtual void print(std::ostream& os) const override;
      
      private:

         Phases phases0_; ///< Phases on bus 0.
         Phases phases1_; ///< Phases on bus 1.
         bool isInService_; ///< Am I in service?

         Event isInServiceChanged_;
         Event admittanceChanged_;
   };
}

#endif // BRANCH_DOT_H
