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

#include "Zip.h"

#include "Bus.h"

#include <ostream>

using namespace arma;

namespace Sgt
{
    Zip::Zip(const std::string& id, const Phases& phases) :
        Component(id),
        phases_(phases),
        YConst_(phases.size(), phases.size(), fill::zeros),
        IConst_(phases.size(), phases.size(), fill::zeros),
        SConst_(phases.size(), phases.size(), fill::zeros)
    {
        // Empty.
    }

    json Zip::toJson() const
    {
        json j = Component::toJson();
		j[sComponentType()] = {
			{"phases", phases()},
			{"YConst", YConst()},
			{"IConst", IConst()},
			{"SConst", SConst()}};
		return j;
	}

    Mat<Complex> Zip::inServiceS() const
    {
        Col<Complex> V = mapPhases(bus_->V(), bus_->phases(), phases_);

        Mat<Complex> SYConst = conj(YConst());
        for (uword i = 0; i < SYConst.n_rows; ++i)
        {
            for (uword k = 0; k < SYConst.n_cols; ++k)
            {
                if (SYConst(i, k) != Complex(0.0))
                {
                    Complex Vik = i == k ? V(i) : V(i) - V(k);
                    SYConst(i, k) *= norm(Vik); // std::norm gives |Vik|^2
                }
            }
        }

        Mat<Complex> SIConst = conj(IConst());
        for (uword i = 0; i < SIConst.n_rows; ++i)
        {
            for (uword k = 0; k < SIConst.n_cols; ++k)
            {
                if (SIConst(i, k) != Complex(0.0))
                {
                    Complex Vik = i == k ? V(i) : V(i) - V(k);
                    SIConst(i, k) *= abs(Vik); // std::norm gives |Vik|^2
                }
            }
        }

        return SYConst + SIConst + SConst();
    }
}
