#ifndef BRANCH_DOT_H
#define BRANCH_DOT_H

#include <SgtCore/Component.h>
#include <SgtCore/Event.h>
#include <SgtCore/PowerFlow.h>

#include <iostream>

namespace SmartGridToolbox
{
   /// @brief Branch is an abstract base class for a branch which connects two Busses in a Network.
   class BranchInterface : virtual public ComponentInterface
   {
      public:

      /// @name Lifecycle:
      /// @{
         
         virtual ~BranchInterface() = default;

      /// @}

      /// @name Phase accessors:
      /// @{
         
         virtual const Phases& phases0() const = 0;
         virtual const Phases& phases1() const = 0;
      
      /// @}
 
      /// @name In service:
      /// @{
         
         virtual bool isInService() const = 0;
         virtual void setIsInService(bool isInService) = 0;

      /// @}

      /// @name Nodal admittance matrix (Y):
      /// @{
         
         virtual const ublas::matrix<Complex> Y() const = 0;
      
      /// @}
      
      /// @name Events.
      /// @{
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& isInServiceChanged() = 0; 

         /// @brief Event triggered when my admittance changes.
         virtual Event& admittanceChanged()  = 0;
         
      /// @}
   };

   /// @brief Common abstract base class for a branch.
   ///
   /// Implement some common functionality for convenience.
   class BranchAbc : public Component, virtual public BranchInterface
   {
      public:

      /// @name Lifecycle:
      /// @{
         
         BranchAbc(const std::string& id, Phases phases0, Phases phases1);

      /// @}
         
      /// @name Component Type:
      /// @{
         
         static constexpr const char* sComponentType()
         {
            return "branch";
         }

         virtual const char* componentType() const override
         {
            return sComponentType();
         }
      
      /// @}

      /// @}

      /// @name Phase accessors:
      /// @{
         
         virtual const Phases& phases0() const override
         {
            return phases0_;
         }
         
         virtual const Phases& phases1() const override
         {
            return phases1_;
         }
         
      /// @}
 
      /// @name In service:
      /// @{
         
         virtual bool isInService() const override
         {
            return isInService_;
         }

         virtual void setIsInService(bool isInService) override
         {
            isInService_ = isInService;
            isInServiceChanged_.trigger();
         }

      /// @}

      /// @name Events.
      /// @{
         
         /// @brief Event triggered when I go in or out of service.
         virtual Event& isInServiceChanged() override
         {
            return isInServiceChanged_;
         }

         /// @brief Event triggered when my admittance changes.
         virtual Event& admittanceChanged() override
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

         Event isInServiceChanged_{std::string(sComponentType()) + " : Is in service changed"};
         Event admittanceChanged_{std::string(sComponentType()) + " : Admittance changed"};
   };

   /// @brief A concrete, generic branch.
   class GenericBranch : public BranchAbc
   {
      public:

      /// @name Lifecycle:
      /// @{

         GenericBranch(const std::string& id, const Phases& phases0, const Phases& phases1) :
            BranchAbc(id, phases0, phases1), Y_(2*phases0.size(), 2*phases0.size(), czero)
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

      /// @name Overridden from Branch:
      /// @{

         virtual const ublas::matrix<Complex> Y() const override
         {
            return Y_;
         }

      /// @}
      
      /// @name Setter for Y.
      /// @{
   
         void setY(const ublas::matrix<Complex>& Y)
         {
            Y_ = Y;
         }

         /// @}

      private:
         
         ublas::matrix<Complex> Y_;
   };
}

#endif // BRANCH_DOT_H
