#ifndef COMMON_BRANCH_DOT_H
#define COMMON_BRANCH_DOT_H

#include <SgtCore/Common.h>
#include <SgtCore/Branch.h>
#include <SgtCore/PowerFlow.h>

#include <iostream>

namespace Sgt
{
   /// @brief A single phase transmission line or transformer.
   class CommonBranch : public BranchAbc
   {
      public:

         SGT_PROPS_INIT(CommonBranch);
         SGT_PROPS_INHERIT(CommonBranch, Component);
         SGT_PROPS_INHERIT(CommonBranch, BranchAbc);

      /// @name Static member functions:
      /// @{
         
         static const std::string& sComponentType()
         {
            static std::string result("common_branch");
            return result;
         }
      
      /// @}

      /// @name Lifecycle:
      /// @{

         CommonBranch(const std::string& id) :
            BranchAbc(id, Phase::BAL, Phase::BAL)
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

         virtual void print(std::ostream& os) const override;

      /// @}

      /// @name Line parameters:
      /// @{
 
         /// Complex tap ratio. 
         ///
         /// tap = V1 / V0 = exp(i theta) (n1 / n0) (where 0 = primary, 1 = secondary).
         /// tap_off_nom = tap * VBase1 / VBase0
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

         double angMin() const
         {
            return angMin_;
         }
         
         virtual void setAngMin(double angMin)
         {
            angMin_ = angMin;
         }

         double angMax() const
         {
            return angMax_;
         }
         
         virtual void setAngMax(double angMax)
         {
            angMax_ = angMax;
         }

      /// @}

      /// @name Overridden from BranchAbc:
      /// @{

         virtual arma::Mat<Complex> inServiceY() const override;

      /// @}

      private:

         Complex tapRatio_{1.0}; // Complex tap ratio, exp(i theta) (n_s / n_p).
         Complex YSeries_{czero}; // Series admittance for top of pi.
         Complex YShunt_{czero}; // Total shunt admittance for both legs of pi - each leg is half of this.

         // The following power ratings are rated to the voltage of the to bus, bus1. They are really current ratings
         // that are multiplied by this voltage.
         double rateA_{infinity};
         double rateB_{infinity};
         double rateC_{infinity};

         double angMin_{-pi};
         double angMax_{pi};
   };
}

#endif // COMMON_BRANCH_DOT_H
