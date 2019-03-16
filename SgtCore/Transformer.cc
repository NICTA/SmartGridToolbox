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

#include "Transformer.h"

using namespace arma;
using namespace std;

namespace Sgt
{
    Mat<Complex> TransformerAbc::inServiceY() const
    {
        ensureValid();
        return Y_;
    }
            
    void TransformerAbc::setNomTurnsRatio(const Col<Complex>& nomTurnsRatio)
    {
        nomTurnsRatio_ = nomTurnsRatio;
        invalidate();
    }

    void TransformerAbc::setNomTurnsRatio(const Complex& nomTurnsRatio, arma::uword iWinding)
    {
        nomTurnsRatio_(iWinding) = nomTurnsRatio;
        invalidate();
    }
             
    void TransformerAbc::setOffNomRatio(const Col<Complex>& offNomRatio)
    {
        offNomRatio_ = offNomRatio;
        invalidate();
    }

    void TransformerAbc::setOffNomRatio(const Complex& offNomRatio, arma::uword iWinding)
    {
        offNomRatio_(iWinding) = offNomRatio;
        invalidate();
    }

    void TransformerAbc::setZL(const Col<Complex>& ZL)
    {
        ZL_ = ZL;
        invalidate();
    }

    void TransformerAbc::setZL(const Complex& ZL, arma::uword iWinding)
    {
        ZL_(iWinding) = ZL;
        invalidate();
    }

    void TransformerAbc::setYM(const Col<Complex>& YM)
    {
        YM_ = YM;
        invalidate();
    }

    void TransformerAbc::setYM(const Complex& YM, arma::uword iWinding)
    {
        YM_(iWinding) = YM;
        invalidate();
    }
    
    void TransformerAbc::invalidate() const
    {
        isValid_ = false;
        admittanceChanged().trigger();
        // TODO: Is this the correct point to do this? Or in ensureValid? Having it here replicates
        // most previous behaviour.
    }

    void TransformerAbc::ensureValid() const
    {
        if (!isValid_)
        {
            Y_ = calcY();
            isValid_ = true;
        }
    }
}
