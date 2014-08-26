#ifndef COMMON_BRANCH_DOT_H
#define COMMON_BRANCH_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/BranchAbc.h>
#include <SgtCore/PowerFlow.h>

#include <iostream>

namespace SmartGridToolbox
{
   class Bus;

   /// @brief A single phase transmission line.
   class CommonBranch : public BranchAbc
   {
      public:

      /// @name Lifecycle:
      /// @{

         CommonBranch(const std::string& id) :
            BranchAbc(id, Phase::BAL, Phase::BAL)
         {
            // Empty.
         }

      /// @}

      /// @name Component Type:
      /// @{

         virtual const char* componentTypeStr() const {return "common_branch";}

      /// @}

      /// @name Line parameters:
      /// @{

         Complex tapRatio() const
         {
            return tapRatio_;
         }

         virtual void setTapRatio(Complex tapRatio)
         {
            tapRatio_ = tapRatio;
         }

         Complex YSeries() const
         {
            return YSeries_;
         }

         virtual void setYSeries(Complex YSeries)
         {
            YSeries_ = YSeries;
         }

         Complex YShunt() const
         {
            return YShunt_;
         }

         virtual void setYShunt(Complex YShunt)
         {
            YShunt_ = YShunt;
         }

         double rateA() const
         {
            return rateA_;
         }

         virtual void setRateA(double rateA)
         {
            rateA_ = rateA;
         }

         double rateB() const
         {
            return rateB_;
         }

         virtual void setRateB(double rateB)
         {
            rateB_ = rateB;
         }

         double rateC() const
         {
            return rateC_;
         }

         virtual void setRateC(double rateC)
         {
            rateC_ = rateC;
         }

      /// @}

      /// @name Overridden from BranchAbc:
      /// @{

         virtual const ublas::matrix<Complex> Y();

      /// @}

      protected:

         virtual void print(std::ostream& os) const override;

      private:

         Complex tapRatio_{1.0}; // Complex tap ratio, exp(i theta) (n_s / n_p).
         Complex YSeries_{czero}; // Series admittance for top of pi.
         Complex YShunt_{czero}; // Total shunt admittance for both legs of pi - each leg is half of this.

         // The following power ratings are rated to the voltage of the to bus, bus1. They are really current ratings
         // that are multiplied by this voltage.
         double rateA_{infinity};
         double rateB_{infinity};
         double rateC_{infinity};
   };
}

#endif // COMMON_BRANCH_DOT_H
