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
    
    void BranchAbc::setIsInService(bool isInService)
    {
        if (isInService != isInService_)
        {
            isInService_ = isInService;
            isInServiceChanged_.trigger();
        }
    }

    array<Col<Complex>, 2> BranchAbc::VBus() const
    {
        return {{mapPhases(bus0()->V(), bus0()->phases(), phases0()), 
            mapPhases(bus1()->V(), bus1()->phases(), phases1())}};
    }
            
    array<Col<Complex>, 2> VIBus(const BranchAbc& branch)
    {
        Col<Complex> V0 = mapPhases(branch.bus0()->V(), branch.bus0()->phases(), branch.phases0());
        Col<Complex> V1 = mapPhases(branch.bus1()->V(), branch.bus1()->phases(), branch.phases1());
        Col<Complex> V = join_vert(V0, V1);
        return {{V, branch.Y() * V}};
    }

    std::array<Col<Complex>, 2> BranchAbc::IBus() const
    {
        auto I = VIBus(*this)[1];

        Col<Complex> I0 = I.subvec(span(0, bus0_->phases().size() - 1));
        I0 = mapPhases(I0, phases0_, bus0_->phases());

        Col<Complex> I1 = I.subvec(span(bus1_->phases().size(), I.size() - 1));
        I1 = mapPhases(I1, phases1_, bus1_->phases());

        return {{I0, I1}};
    }

    std::array<Col<Complex>, 2> BranchAbc::SBus() const
    {
        auto VI = VIBus(*this);
        Col<Complex> S = VI[0] % conj(VI[1]);

        Col<Complex> S0 = S.subvec(span(0, bus0_->phases().size() - 1));
        S0 = mapPhases(S0, phases0_, bus0_->phases());
        
        Col<Complex> S1 = S.subvec(span(bus1_->phases().size(), S.size() - 1));
        S1 = mapPhases(S1, phases1_, bus1_->phases());

        return {{S0, S1}};
    }
}
