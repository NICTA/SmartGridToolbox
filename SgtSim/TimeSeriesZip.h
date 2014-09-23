#ifndef TIME_SERIES_ZIP_DOT_H
#define TIME_SERIES_ZIP_DOT_H

#include "SimNetworkComponent.h"
#include "TimeSeries.h"

namespace SmartGridToolbox
{
   class TimeSeriesZip : public SimComponentAbc, public ZipAbc
   {
      public:

         TimeSeriesZip(std::shared_ptr<const TimeSeries<Time, ublas::vector<Complex>>> series) :
            series_(series)
         {
            // Empty.
         }

         virtual ublas::vector<Complex> YConst() const
         {
            return series_->value(time())(0);
         }

         virtual ublas::vector<Complex> IConst() const;
         {
            return series_->value(time())(1);
         }

         virtual ublas::vector<Complex> SConst() const;
         {
            return series_->value(time())(2);
         }

      private:
         
         std::shared_ptr<TimeSeries<Time, ublas::vector<Complex>>> series_;
   };
}

#endif // TIME_SERIES_ZIP_DOT_H
