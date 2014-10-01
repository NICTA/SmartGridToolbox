#ifndef TIME_SERIES_ZIP_DOT_H
#define TIME_SERIES_ZIP_DOT_H

#include "RegularUpdateComponent.h"
#include "SimNetworkComponent.h"
#include "TimeSeries.h"

namespace SmartGridToolbox
{
   class TimeSeriesZip : public ZipAbc, public RegularUpdateComponentAbc 
   {
      public:

         TimeSeriesZip(const std::string& id, const Phases& phases,
               std::shared_ptr<const TimeSeries<Time, ublas::vector<Complex>>> series, const Time& dt) :
            ZipAbc(id, phases),
            RegularUpdateComponentAbc(dt),
            series_(series)
         {
            // Empty.
         }

         virtual ublas::vector<Complex> YConst() const
         {
            return ublas::project(series_->value(lastUpdated()), ublas::range(0, phases().size()));
         }

         virtual ublas::vector<Complex> IConst() const
         {
            return ublas::project(series_->value(lastUpdated()), ublas::range(phases().size(), 2*phases().size()));
         }

         virtual ublas::vector<Complex> SConst() const
         {
            return ublas::project(series_->value(lastUpdated()), ublas::range(2*phases().size(), 3*phases().size()));
         }

      protected:

         virtual void updateState(Time t)
         {
            RegularUpdateComponentAbc::updateState(t);
            injectionChanged().trigger(); 
         }

      private:
         
         std::shared_ptr<const TimeSeries<Time, ublas::vector<Complex>>> series_;
   };
}

#endif // TIME_SERIES_ZIP_DOT_H
