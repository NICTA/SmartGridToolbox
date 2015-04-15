#ifndef TIME_SERIES_ZIP_DOT_H
#define TIME_SERIES_ZIP_DOT_H

#include "Heartbeat.h"
#include "SimNetworkComponent.h"
#include "TimeSeries.h"

namespace Sgt
{
   class TimeSeriesZip : public ZipAbc, public HeartbeatAdaptor
   {
      public:

         TimeSeriesZip(const std::string& id, const Phases& phases,
                       std::shared_ptr<const TimeSeries<Time, arma::Col<Complex>>> series, const Time& dt) :
            ZipAbc(id, phases),
            HeartbeatAdaptor(dt),
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
            HeartbeatAdaptor::updateState(t);
            injectionChanged().trigger();
         }

      private:

         std::shared_ptr<const TimeSeries<Time, arma::Col<Complex>>> series_;
   };
}

#endif // TIME_SERIES_ZIP_DOT_H
