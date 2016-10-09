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
                    const ConstTimeSeriesPtr<TimeSeries<Time, arma::Col<Complex>>>& series, const Time& dt,
                    const arma::Mat<arma::uword>& matrixElems,
                    const arma::Col<arma::uword>& dataIdxsY,
                    const arma::Col<arma::uword>& dataIdxsI,
                    const arma::Col<arma::uword>& dataIdxsS);

            virtual const std::string& componentType() const override
            {
                return sComponentType();
            }

            virtual const ZipAbc& zip() const override
            {
                return *this;
            }

            virtual ZipAbc& zip() override
            {
                return *this;
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
            virtual void updateState(Time t) override;
            
            virtual arma::Mat<Complex> YConst() const override;
            virtual arma::Mat<Complex> IConst() const override;
            virtual arma::Mat<Complex> SConst() const override;

        private:
            arma::Mat<Complex> mapToMat(const arma::Col<Complex>& vec) const;

        private:
            ConstTimeSeriesPtr<TimeSeries<Time, arma::Col<Complex>>> series_;
            arma::Mat<arma::uword> matrixElems_;
            arma::Col<arma::uword> dataIdxsY_;
            arma::Col<arma::uword> dataIdxsI_;
            arma::Col<arma::uword> dataIdxsS_;
            double scaleFactorY_{1.0};
            double scaleFactorI_{1.0};
            double scaleFactorS_{1.0};
    };
}

#endif // TIME_SERIES_ZIP_DOT_H
