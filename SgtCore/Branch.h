#ifndef BRANCH_DOT_H
#define BRANCH_DOT_H

#include <SgtCore/Component.h>
#include <SgtCore/Event.h>
#include <SgtCore/PowerFlow.h>

#include <iostream>

namespace Sgt
{
   class Bus;

   /// @brief Common abstract base class for a branch.
   ///
   /// Implement some common functionality for convenience.
   class BranchAbc : public Component
   {
      friend class Network;

      public:

         SGT_PROPS_INIT(BranchAbc);
         SGT_PROPS_INHERIT(BranchAbc, Component);

      /// @name Static member functions:
      /// @{
         
         static const std::string& sComponentType()
         {
            static std::string result("branch");
            return result;
         }
      
      /// @}

      /// @name Lifecycle:
      /// @{
         
         BranchAbc(const std::string& id, const Phases& phases0, const Phases& phases1);

      /// @}
            
      /// @name ComponentInterface virtual overridden functions.
      /// @{

         virtual const std::string& componentType() const override
         {
            return sComponentType();
         }

         virtual void print(std::ostream& os) const override;

      /// @}
     
      /// @name Phase accessors:
      /// @{
         
         virtual const Phases& phases0() const
         {
            return phases0_;
         }

         SGT_PROP_GET(phases0, BranchAbc, Phases, GetByConstRef, phases0);
         
         virtual const Phases& phases1() const
         {
            return phases1_;
         }
         
         SGT_PROP_GET(phases1, BranchAbc, Phases, GetByConstRef, phases1);

      /// @}
        
      /// @name attached busses:
      /// @{
         
         std::shared_ptr<const Bus> bus0() const
         {
            return bus0_;
         }

         std::shared_ptr<Bus> bus0()
         {
            return bus0_;
         }

         void setBus0(std::shared_ptr<Bus> bus0)
         {
            bus0_ = bus0;
         }

         std::shared_ptr<const Bus> bus1() const
         {
            return bus1_;
         }

         std::shared_ptr<Bus> bus1()
         {
            return bus1_;
         }

         void setBus1(std::shared_ptr<Bus> bus1)
         {
            bus1_ = bus1;
         }

      /// @}
      
      /// @name In service:
      /// @{
         
         virtual bool isInService() const
         {
            return isInService_;
         }

         virtual void setIsInService(bool isInService)
         {
            isInService_ = isInService;
            isInServiceChanged_.trigger();
         }

         SGT_PROP_GET_SET(isInService, BranchAbc, bool, GetByVal, isInService, setIsInService);

      /// @}

      /// @name Nodal admittance matrix (Y):
      /// @{
         
         virtual arma::Mat<Complex> Y() const final
         {
            return isInService_ ? inServiceY() : arma::Mat<Complex>(
                  2 * phases0_.size(), 2 * phases1_.size(), arma::fill::zeros);
         }

         SGT_PROP_GET(Y, BranchAbc, arma::Mat<Complex>, GetByVal, Y);

         /// @brief The admittance whenever isInService.
         virtual arma::Mat<Complex> inServiceY() const = 0;
      
         SGT_PROP_GET(inServiceY, BranchAbc, arma::Mat<Complex>, GetByVal, inServiceY);

      /// @}
      
      /// @name Events.
      /// @{
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& isInServiceChanged()
         {
            return isInServiceChanged_;
         }

         /// @brief Event triggered when my admittance changes.
         virtual Event& admittanceChanged()
         {
            return admittanceChanged_;
         }
         
      /// @}
      
      private:

         Phases phases0_; ///< Phases on bus 0.
         Phases phases1_; ///< Phases on bus 1.

         std::shared_ptr<Bus> bus0_;
         std::shared_ptr<Bus> bus1_;

         bool isInService_; ///< Am I in service?

         Event isInServiceChanged_{sComponentType() + " : Is in service changed"};
         Event admittanceChanged_{sComponentType() + " : Admittance changed"};
   };

   /// @brief A concrete, generic branch.
   class GenericBranch : public BranchAbc
   {
      public:
         
         SGT_PROPS_INIT(GenericBranch);
         SGT_PROPS_INHERIT(GenericBranch, Component);

      /// @name Static member functions:
      /// @{
         
         static const std::string& sComponentType()
         {
            static std::string result("generic_branch");
            return result;
         }
      
      /// @}
      
      /// @name Lifecycle:
      /// @{

         GenericBranch(const std::string& id, const Phases& phases0, const Phases& phases1) :
            BranchAbc(id, phases0, phases1), Y_(2*phases0.size(), 2*phases0.size(), arma::fill::zeros)
         {
            // Empty.
         }

      /// @}
       
      /// @name ComponentInterface virtual overridden functions.
      /// @{

         virtual const std::string& componentType() const override
         {
            return sComponentType();
         }

         // virtual void print(std::ostream& os) const override; // TODO

      /// @}
         
      /// @name BranchAbc virtual overridden functions.
      /// @{

         virtual arma::Mat<Complex> inServiceY() const override
         {
            return Y_;
         }

      /// @}
      
      /// @name Setter for Y.
      /// @{
   
         void setInServiceY(const arma::Mat<Complex>& Y)
         {
            Y_ = Y;
         }

         /// @}
      
      protected:

      private:
         
         arma::Mat<Complex> Y_;
   };
}

#endif // BRANCH_DOT_H
