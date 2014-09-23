#ifndef TIME_SERIES_ZIP_DOT_H
#define TIME_SERIES_ZIP_DOT_H

#include "SimNetworkComponent.h"
#include "TimeSeries.h"

namespace SmartGridToolbox
{
   class TimeSeriesZip : public SimComponentAbc, public ZipAbc
   {
      public:

         TimeSeriesZip(const std::string& id, const Phases& phases,
               std::shared_ptr<const TimeSeries<Time, ublas::vector<Complex>>> series) :
            ZipAbc(id, phases),
            series_(series)
         {
            // Empty.
         }

         virtual ublas::vector<Complex> YConst() const
         {
            return ublas::project(series_->value(time()), ublas::range(0, phases().size()));
         }

         virtual ublas::vector<Complex> IConst() const
         {
            return ublas::project(series_->value(time()), ublas::range(phases().size(), 2*phases().size()));
         }

         virtual ublas::vector<Complex> SConst() const
         {
            return ublas::project(series_->value(time()), ublas::range(2*phases().size(), 3*phases().size()));
         }

      private:
         
         std::shared_ptr<const TimeSeries<Time, ublas::vector<Complex>>> series_;
   };
}

#endif // TIME_SERIES_ZIP_DOT_H
