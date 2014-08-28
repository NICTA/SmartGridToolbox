#ifndef GENERIC_BRANCH_DOT_H
#define GENERIC_BRANCH_DOT_H

#include <SgtCore/BranchAbc.h>

namespace SmartGridToolbox
{
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

         virtual const char* componentTypeStr() const {return "generic_branch";}

      /// @}

      /// @name Overridden from BranchAbc:
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

#endif // GENERIC_BRANCH_DOT_H
