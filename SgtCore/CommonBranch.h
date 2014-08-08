#ifndef COMMON_BRANCH_DOT_H
#define COMMON_BRANCH_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/Branch.h>
#include <SgtCore/PowerFlow.h>

#include <iostream>

namespace SmartGridToolbox
{
   class Bus;

   /// @brief A single phase transmission line.
   class CommonBranch : public Branch
   {
      public:
      
      /// @name Lifecycle:
      /// @{
         
         CommonBranch(const std::string& id) :
            Branch(id, Phase::BAL, Phase::BAL)
         {
            // Empty.
         }

      /// @}
      
      /// @name Line parameters:
      /// @{
         
         Complex tapRatio() const
         {
            return tapRatio_;
         }

         void setTapRatio(Complex tapRatio)
         {
            tapRatio_ = tapRatio;
         }
         
         Complex ySeries() const
         {
            return ySeries_;
         }

         void set_ySeries(Complex ySeries)
         {
            ySeries_ = ySeries;
         }
         
         Complex yShunt() const
         {
            return yShunt_;
         }

         void set_yShunt(Complex yShunt)
         {
            yShunt_ = yShunt;
         }

         double rateA() const
         {
            return rateA_;
         }

         void setRateA(double rateA)
         {
            rateA_ = rateA;
         }

         double rateB() const
         {
            return rateB_;
         }

         void setRateB(double rateB)
         {
            rateB_ = rateB;
         }

         double rateC() const
         {
            return rateC_;
         }

         void setRateC(double rateC)
         {
            rateC_ = rateC;
         }

      /// @}
      
      /// @name Overridden from Branch:
      /// @{
         
         virtual const ublas::matrix<Complex> Y();

      /// @}
         
      protected:

         virtual std::ostream& print(std::ostream& os) const;
      
      private:

         Complex tapRatio_{1.0}; // Complex tap ratio, exp(i theta) (n_s / n_p).
         Complex ySeries_{czero}; // Series admittance for top of pi.
         Complex yShunt_{czero}; // Total shunt admittance for both legs of pi - each leg is half of this.

         // The following power ratings are rated to the voltage of the to bus, bus1. They are really current ratings
         // that are multiplied by this voltage.
         double rateA_{infinity};
         double rateB_{infinity};
         double rateC_{infinity};
   };
}

#endif // COMMON_BRANCH_DOT_H
