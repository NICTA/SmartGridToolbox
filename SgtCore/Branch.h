#ifndef BRANCH_DOT_H
#define BRANCH_DOT_H

#include <SgtCore/Component.h>
#include <SgtCore/Event.h>
#include <SgtCore/PowerFlow.h>

#include <iostream>

namespace SmartGridToolbox
{
   /// @brief Common abstract base class for a branch.
   ///
   /// Implement some common functionality for convenience.
   class BranchAbc : virtual public ComponentInterface
   {
      public:

         SGT_PROPS_INIT(BranchAbc);
         SGT_PROPS_INHERIT(BranchAbc, Component);

      /// @name Lifecycle:
      /// @{
         
         BranchAbc(const Phases& phases0, const Phases& phases1);

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
      
      protected:

         virtual void print(std::ostream& os) const override;
      
      private:

         Phases phases0_; ///< Phases on bus 0.
         Phases phases1_; ///< Phases on bus 1.
         bool isInService_; ///< Am I in service?

         Event isInServiceChanged_{std::string(componentType()) + " : Is in service changed"};
         Event admittanceChanged_{std::string(componentType()) + " : Admittance changed"};
   };

   /// @brief A concrete, generic branch.
   class GenericBranch : public Component, public BranchAbc
   {
      public:
         
         SGT_PROPS_INIT(GenericBranch);
         SGT_PROPS_INHERIT(GenericBranch, Component);

      /// @name Lifecycle:
      /// @{

         GenericBranch(const std::string& id, const Phases& phases0, const Phases& phases1) :
            Component(id), BranchAbc(phases0, phases1), Y_(2*phases0.size(), 2*phases0.size(), arma::fill::zeros)
         {
            // Empty.
         }

      /// @}
      
      /// @name Component Type:
      /// @{
         
         static constexpr const char* sComponentType()
         {
            return "generic_branch";
         }

         virtual const char* componentType() const override
         {
            return sComponentType();
         }

      /// @}

      /// @name Overridden from BranchAbc:
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

         virtual void print(std::ostream& os) const override
         {
            BranchAbc::print(os);
         }

      private:
         
         arma::Mat<Complex> Y_;
   };
}

#endif // BRANCH_DOT_H
