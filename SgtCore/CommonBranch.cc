#include "CommonBranch.h"

namespace Sgt
{
   arma::Mat<Complex> CommonBranch::inServiceY() const
   {
      if (!isValid_)
      {
         validate();
      }
      return Y_;
   }

   void CommonBranch::print(std::ostream& os) const
   {
      BranchAbc::print(os);
      StreamIndent _(os);
      os << "tap_ratio_magnitude: " << std::abs(tapRatio_) << std::endl;
      os << "tap_ratio_angle_deg: " << std::arg(tapRatio_) * 180 / pi << std::endl;
      os << "Y_series: " << YSeries_ << std::endl;
      os << "Y_shunt: " << YShunt_ << std::endl;
      os << "rate_A: " << rateA_ << std::endl;
      os << "rate_B: " << rateB_ << std::endl;
      os << "rate_C: " << rateC_ << std::endl;
   }

   void CommonBranch::validate() const
   {
      if (!isValid_)
      {
         Y_ = arma::Mat<Complex>(2, 2, arma::fill::none);
         Complex c1 = YSeries_ + 0.5 * YShunt_;
         Y_(0, 0) = c1 / norm(tapRatio_); // Yes, C++ does define norm(c) as |c|^2 - field norm?
         Y_(0, 1) = -YSeries_ / conj(tapRatio_);
         Y_(1, 0) = -YSeries_ / tapRatio_;
         Y_(1, 1) = c1;

         isValid_ = true;
      }
   }
}
