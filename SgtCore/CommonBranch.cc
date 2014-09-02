#include "CommonBranch.h"

namespace SmartGridToolbox
{
   const ublas::matrix<Complex> CommonBranch::Y() const
   {
      ublas::matrix<Complex> result(2, 2);

      Complex c1 = YSeries_ + 0.5 * YShunt_;
      result(0, 0) = c1 / norm(tapRatio_); // Yes, C++ does define norm(c) as |c|^2 - field norm?
      result(0, 1) = -YSeries_ / conj(tapRatio_);
      result(1, 0) = -YSeries_ / tapRatio_;
      result(1, 1) = c1;

      return result;
   }

   void CommonBranch::print(std::ostream& os) const
   {
      BranchInterface::print(os);
      IndentingOStreamBuf _(os);
      os << "tap_ratio_magnitude: " << std::abs(tapRatio_) << std::endl;
      os << "tap_ratio_angle_deg: " << std::arg(tapRatio_) * 180 / pi << std::endl;
      os << "Y_series: " << YSeries_ << std::endl;
      os << "Y_shunt: " << YShunt_ << std::endl;
      os << "rate_A: " << rateA_ << std::endl;
      os << "rate_B: " << rateB_ << std::endl;
      os << "rate_C: " << rateC_ << std::endl;
   }
}
