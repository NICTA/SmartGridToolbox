#ifndef COMMON_DOT_H
#define COMMON_DOT_H

#include <cmath>
#include <complex>
#include <string>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

namespace SmartGridToolbox
{
   using namespace boost::posix_time;
   using namespace boost::gregorian;
   typedef std::complex<double> Complex;
}

#endif // COMMON_DOT_H
