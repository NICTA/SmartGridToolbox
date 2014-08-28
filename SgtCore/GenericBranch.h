#ifndef GENERIC_BRANCH_DOT_H
#define GENERIC_BRANCH_DOT_H

#include <SgtCore/Branch.h>

namespace SmartGridToolbox
{
   class GenericBranch : public Branch
   {
      public:

      /// @name Lifecycle:
      /// @{

         GenericBranch(const std::string& id, const Phases& phases0, const Phases& phases1) :
            Branch(id, phases0, phases1), Y_(2*phases0.size(), 2*phases0.size(), czero)
         {
            // Empty.
         }

      /// @}
      
      /// @name Component Type:
      /// @{

         virtual const char* componentTypeStr() const {return "generic_branch";}

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

#endif // GENERIC_BRANCH_DOT_H
