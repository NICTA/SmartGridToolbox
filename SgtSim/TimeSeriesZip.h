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
#include "TimeSeries.h"

#include <SgtSim/SimZip.h>

namespace Sgt
{
    class TimeSeriesZip : public SimZipAbc, public Heartbeat, private ZipAbc
    {
        public:
            
            static const std::string& sComponentType()
            {
                static std::string result("time_series_zip");
                return result;
            }

        public:

            TimeSeriesZip(const std::string& id, const Phases& phases,
                          std::shared_ptr<const TimeSeries<Time, arma::Col<Complex>>> series, const Time& dt) :
                Component(id),
                Heartbeat(id, dt),
                ZipAbc(phases),
                series_(series)
            {
                // Empty.
            }

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            virtual void print(std::ostream& os) const override;

            virtual std::shared_ptr<const ZipAbc> zip() const override
            {
                return shared<const ZipAbc>();
            }

            virtual std::shared_ptr<ZipAbc> zip() override
            {
                return shared<ZipAbc>();
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

            virtual void updateState(Time t) override
            {
                Heartbeat::updateState(t);
                injectionChanged().trigger();
            }

            virtual arma::Col<Complex> YConst() const override
            {
                return scaleFactorY_ * series_->value(lastUpdated())(
                        arma::span(0, phases().size() - 1));
            }

            virtual arma::Col<Complex> IConst() const override
            {
                return scaleFactorI_ * series_->value(lastUpdated())(
                        arma::span(phases().size(), 2 * phases().size() - 1));
            }

            virtual arma::Col<Complex> SConst() const override
            {
                return scaleFactorS_ * series_->value(lastUpdated())(
                        arma::span(2 * phases().size(), 3 * phases().size() - 1));
            }

        private:

            std::shared_ptr<const TimeSeries<Time, arma::Col<Complex>>> series_;
            double scaleFactorY_{1.0};
            double scaleFactorI_{1.0};
            double scaleFactorS_{1.0};
    };
}

#endif // TIME_SERIES_ZIP_DOT_H
