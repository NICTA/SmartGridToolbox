#include "CommonBranch.h"

namespace SmartGridToolbox
{
   const ublas::matrix<Complex> CommonBranch::Y()
   {
      ublas::matrix<Complex> result(2, 2);

      Complex c1 = ySeries_ + 0.5*yShunt_;
      result(0, 0) = c1/norm(tapRatio_); // Yes, C++ does define norm(c) as |c|^2 - field norm?
      result(0, 1) = -ySeries_/conj(tapRatio_);
      result(1, 0) = -ySeries_/tapRatio_;
      result(1, 1) = c1;

      return result;
   }

   std::ostream& CommonBranch::print(std::ostream& os) const
   {
      IndentingOStreamBuf ind(os, "");
      os << "common_branch:" << std::endl;
      ind.setInd("    ");
      Branch::print(os);
      os << "tap_ratio_magnitude: " << std::abs(tapRatio_) << std::endl;
      os << "tap_ratio_angle_deg: " << std::arg(tapRatio_)*180/pi << std::endl;
      os << "y_series: " << ySeries_ << std::endl;
      os << "y_shunt: " << yShunt_ << std::endl;
      os << "rate_A: " << rateA_ << std::endl;
      os << "rate_B: " << rateB_ << std::endl;
      os << "rate_C: " << rateC_ << std::endl;
      return os;
   }
}
