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

namespace Sgt
{
    BranchAbc::BranchAbc(const Phases& phases0, const Phases& phases1) :
        phases0_(phases0),
        phases1_(phases1),
        isInService_(true)
    {
        // Empty.
    }

    void BranchAbc::print(std::ostream& os) const
    {
        Component::print(os);
        StreamIndent _(os);
        os << "phases0: " << phases0() << std::endl;
        os << "phases1: " << phases1() << std::endl;
    }
    
    json BranchAbc::asJson() const
    {
        json j = this->Component::asJson();
        j[sComponentType()] = {
            {"phases0", toJson(phases0())},
            {"phases1", toJson(phases1())},
            {"bus0", bus0()->id()},
            {"bus1", bus1()->id()},
            {"is_in_service", toJson(isInService())},
            {"Y", toJson(Y())}
        };
        return j;
    }
}
