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

#include "TimeSeriesZip.h"

using namespace arma;

namespace
{
}

namespace Sgt
{
    TimeSeriesZip::TimeSeriesZip(const std::string& id, const Phases& phases,
                    const TimeSeries<Time, Col<Complex>>* series, const Time& dt,
                    const Mat<uword>& matrixElems,
                    const Col<uword>& dataIdxsY,
                    const Col<uword>& dataIdxsI,
                    const Col<uword>& dataIdxsS) :
        Component(id),
        Heartbeat(id, dt),
        ZipAbc(phases),
        series_(series),
        matrixElems_(matrixElems),
        dataIdxsY_(dataIdxsY),
        dataIdxsI_(dataIdxsI),
        dataIdxsS_(dataIdxsS)
    {
        if (matrixElems_.size() == 0)
        {
            // Default is Y load, and assume n columns in data series.
            matrixElems_ = Mat<uword>(phases.size(), 2, fill::none);
            for (uword i = 0; i < phases.size(); ++i)
            {
                matrixElems_(i, 0) = matrixElems_(i, 1) = i;
            }
        }
        sgtAssert(matrixElems_.n_cols == 2, "TimeSeriesZip: phase indices parameter must be n x 2.");
    }
            
    void TimeSeriesZip::updateState(Time t)
    {
        Heartbeat::updateState(t);
        injectionChanged().trigger();
    }

    Mat<Complex> TimeSeriesZip::YConst() const
    {
        return dataIdxsY_.size() == 0 
            ? Mat<Complex>(phases().size(), phases().size(), fill::zeros)
            : mapToMat(scaleFactorY_ * series_->value(lastUpdated())(dataIdxsY_));
    }

    Mat<Complex> TimeSeriesZip::IConst() const
    {
        return dataIdxsI_.size() == 0 
            ? Mat<Complex>(phases().size(), phases().size(), fill::zeros)
            : mapToMat(scaleFactorY_ * series_->value(lastUpdated())(dataIdxsI_));
    }

    Mat<Complex> TimeSeriesZip::SConst() const
    {
        return dataIdxsS_.size() == 0 
            ? Mat<Complex>(phases().size(), phases().size(), fill::zeros)
            : mapToMat(scaleFactorY_ * series_->value(lastUpdated())(dataIdxsS_));
    }

    Mat<Complex> TimeSeriesZip::mapToMat(const Col<Complex>& vec) const
    {
        auto n = phases().size();
        Mat<Complex> result(n, n, fill::zeros);
        for (uword i = 0; i < vec.size(); ++i) result(matrixElems_(i, 0), matrixElems_(i, 1)) = vec(i);
        return result;
    }
}
