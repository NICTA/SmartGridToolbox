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

#include "Branch.h"
#include "Bus.h"

#include <ostream>

using namespace arma;
using namespace std;

namespace Sgt
{
    BranchAbc::BranchAbc(const Phases& phases0, const Phases& phases1) :
        phases0_(phases0),
        phases1_(phases1),
        isInService_(true)
    {
        // Empty.
    }

    json BranchAbc::toJson() const
    {
        json j = this->Component::toJson();
        j[sComponentType()] = {
            {"phases0", phases0()},
            {"phases1", phases1()},
            {"bus0", bus0()->id()},
            {"bus1", bus1()->id()},
            {"is_in_service", isInService()},
            {"Y", Y()}
        };
        return j;
    }
            
    Col<Complex> lineCurrents(const BranchAbc& branch)
    {
        sgtAssert(branch.phases0().size() == branch.phases1().size(),
                "lineCurrents(...) requires matching phases on both buses.");
        size_t nPhase = branch.phases0().size();
        Col<Complex> deltaV = branch.bus0()->V() - branch.bus1()->V();
        Col<Complex> y(nPhase, fill::none);
        Mat<Complex> YBus = branch.Y();
        for (size_t i = 0; i < nPhase; ++i)
        {
            y(i) = -YBus(i, i + nPhase);
        }
        return y % deltaV;
    }
}
