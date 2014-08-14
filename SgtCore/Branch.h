#ifndef BRANCH_DOT_H
#define BRANCH_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/Component.h>
#include <SgtCore/PowerFlow.h>

#include <iostream>

namespace SmartGridToolbox
{
   class Bus;

   /// @brief A Branch connects two Busses in a Network.
   class Branch : public Component
   {
      public:

      /// @name Lifecycle:
      /// @{
         
         Branch(const std::string& id, Phases phases0, Phases phases1);

         virtual ~Branch() = default;
      
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
 
      /// @name Status:
      /// @{
         
         bool status() const
         {
            return status_;
         }

         virtual void setStatus(bool status)
         {
            status_ = status;
         }

      /// @}
        
      /// @name Bus accessors:
      /// @{
         
         const Bus& bus0() const
         {
            return *bus0_;
         }

         virtual void setBus0(Bus& bus0)
         {
            bus0_ = &bus0;
         }
          
         const Bus& bus1() const
         {
            return *bus1_;
         }

         virtual void setBus1(Bus& bus1)
         {
            bus1_ = &bus1;
         }
     
      /// @}

      /// @name Nodal admittance matrix (Y):
      /// @{
         
         virtual const ublas::matrix<Complex> Y() = 0;
      
      /// @}
      
      protected:

         virtual void print(std::ostream& os) const override;
      
      private:

         Phases phases0_; ///< Phases on bus 0.
         Phases phases1_; ///< Phases on bus 1.
         bool status_;
         Bus* bus0_{nullptr}; ///< My bus 0.
         Bus* bus1_{nullptr}; ///< My bus 1.
   };
}

#endif // BRANCH_DOT_H
