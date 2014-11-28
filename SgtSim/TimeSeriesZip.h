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
               std::shared_ptr<const TimeSeries<Time, arma::Col<Complex>>> series, const Time& dt) :
            ZipAbc(id, phases),
            RegularUpdateComponentAbc(dt),
            series_(series)
         {
            // Empty.
         }

         virtual arma::Col<Complex> YConst() const
         {
            return series_->value(lastUpdated())(arma::span(0, phases().size() - 1));
         }

         virtual arma::Col<Complex> IConst() const
         {
            return series_->value(lastUpdated())(arma::span(phases().size(), 2 * phases().size() - 1));
         }

         virtual arma::Col<Complex> SConst() const
         {
            return series_->value(lastUpdated())(arma::span(2 * phases().size(), 3 * phases().size() - 1));
         }

      protected:

         virtual void updateState(Time t)
         {
            RegularUpdateComponentAbc::updateState(t);
            injectionChanged().trigger(); 
         }

      private:
         
         std::shared_ptr<const TimeSeries<Time, arma::Col<Complex>>> series_;
   };
}

#endif // TIME_SERIES_ZIP_DOT_H
