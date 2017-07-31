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

namespace Sgt
{
    class Zip;

    class TimeSeriesZip : public Heartbeat
    {
        public:
            static const std::string& sComponentType()
            {
                static std::string result("time_series_zip");
                return result;
            }

        public:

            /// @brief Constructor.
            ///
            /// @param matrixElems A n x 2 matrix giving row and col indices for non-zero matrix elements. 
            /// @param dataIdxsY Indices into time series data of const Y component. Empty or of size matrixElems.
            /// @param dataIdxsI Indices into time series data of const I component. Empty or of size matrixElems.
            /// @param dataIdxsS Indices into time series data of const S component. Empty or of size matrixElems.
            TimeSeriesZip(const std::string& id, const ComponentPtr<Zip>& zip,
                    const ConstTimeSeriesPtr<TimeSeries<Time, arma::Col<Complex>>>& series, const Time& dt,
                    const arma::Mat<arma::uword>& matrixElems,
                    const arma::Col<arma::uword>& dataIdxsY,
                    const arma::Col<arma::uword>& dataIdxsI,
                    const arma::Col<arma::uword>& dataIdxsS);

            virtual const std::string& componentType() const override
            {
                return sComponentType();
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
            
        private:
            arma::Mat<Complex> YConst() const;
            arma::Mat<Complex> IConst() const;
            arma::Mat<Complex> SConst() const;

            arma::Mat<Complex> mapToMat(const arma::Col<Complex>& vec) const;

        private:
            ComponentPtr<Zip> zip_;
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
