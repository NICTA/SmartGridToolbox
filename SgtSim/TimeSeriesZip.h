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
                return scaleFactorY_ * series_->value(lastUpdated())(
                        arma::span(0, phases().size() - 1));
            }

            virtual arma::Col<Complex> IConst() const
            {
                return scaleFactorI_ * series_->value(lastUpdated())(
                        arma::span(phases().size(), 2 * phases().size() - 1));
            }

            virtual arma::Col<Complex> SConst() const
            {
                return scaleFactorS_ * series_->value(lastUpdated())(
                        arma::span(2 * phases().size(), 3 * phases().size() - 1));
            }

            double scaleFactorY() const
            {
                return scaleFactorY_;
            }
            
            void setScaleFactorY(double scaleFactorY)
            {
                scaleFactorY_ = scaleFactorY;
            }

            double scaleFactorI() const
            {
                return scaleFactorI_;
            }
            
            void setScaleFactorI(double scaleFactorI)
            {
                scaleFactorI_ = scaleFactorI;
            }

            double scaleFactorS() const
            {
                return scaleFactorS_;
            }
            
            void setScaleFactorS(double scaleFactorS)
            {
                scaleFactorS_ = scaleFactorS;
            }

        protected:

            virtual void updateState(Time t)
            {
                HeartbeatAdaptor::updateState(t);
                injectionChanged().trigger();
            }

        private:

            std::shared_ptr<const TimeSeries<Time, arma::Col<Complex>>> series_;
            double scaleFactorY_{1.0};
            double scaleFactorI_{1.0};
            double scaleFactorS_{1.0};
    };
}

#endif // TIME_SERIES_ZIP_DOT_H
