// Copyright 2015-2016 National ICT Australia Limited (NICTA)
// Copyright 2016-2019 The Australian National University
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

#include "TimeSeriesZip.h"

#include <SgtCore/Zip.h>

using namespace arma;

namespace Sgt
{
    TimeSeriesZip::TimeSeriesZip(const std::string& id, const ComponentPtr<Zip>& zip,
                    const ConstTimeSeriesPtr<TimeSeries<Time, Col<Complex>>>& series, const Time& dt,
                    const Mat<uword>& matrixElems,
                    const Col<uword>& dataIdxsY,
                    const Col<uword>& dataIdxsI,
                    const Col<uword>& dataIdxsS) :
        Component(id),
        Heartbeat(dt),
        SimZip(zip),
        series_(series),
        matrixElems_(matrixElems),
        dataIdxsY_(dataIdxsY),
        dataIdxsI_(dataIdxsI),
        dataIdxsS_(dataIdxsS)
    {
        uword nPhase = zip->phases().size();
        if (matrixElems_.size() == 0)
        {
            // Default is Y load, and assume n columns in data series.
            matrixElems_ = Mat<uword>(nPhase, 2, fill::none);
            for (uword i = 0; i < nPhase; ++i)
            {
                matrixElems_(i, 0) = matrixElems_(i, 1) = i;
            }
        }
        sgtAssert(matrixElems_.n_cols == 2, "TimeSeriesZip: matrixElems parameter must be n x 2.");
    }
            
    void TimeSeriesZip::updateState(const Time& t)
    {
        Heartbeat::updateState(t);
        zip()->setYConst(YConst(t));
        zip()->setIConst(IConst(t));
        zip()->setSConst(SConst(t));
    }

    Mat<Complex> TimeSeriesZip::YConst(const Time& t) const
    {
        uword nPhase = zip()->phases().size();
        return dataIdxsY_.size() == 0 
            ? Mat<Complex>(nPhase, nPhase, fill::zeros)
            : mapToMat(scaleFactorY_ * series_->value(t)(dataIdxsY_));
    }

    Mat<Complex> TimeSeriesZip::IConst(const Time& t) const
    {
        uword nPhase = zip()->phases().size();
        return dataIdxsI_.size() == 0 
            ? Mat<Complex>(nPhase, nPhase, fill::zeros)
            : mapToMat(scaleFactorI_ * series_->value(t)(dataIdxsI_));
    }

    Mat<Complex> TimeSeriesZip::SConst(const Time& t) const
    {
        uword nPhase = zip()->phases().size();
        return dataIdxsS_.size() == 0 
            ? Mat<Complex>(nPhase, nPhase, fill::zeros)
            : mapToMat(scaleFactorS_ * series_->value(t)(dataIdxsS_));
    }

    Mat<Complex> TimeSeriesZip::mapToMat(const Col<Complex>& vec) const
    {
        uword nPhase = zip()->phases().size();
        Mat<Complex> result(nPhase, nPhase, fill::zeros);
        for (uword i = 0; i < vec.size(); ++i) result(matrixElems_(i, 0), matrixElems_(i, 1)) = vec(i);
        return result;
    }
}
