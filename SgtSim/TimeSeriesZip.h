// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
