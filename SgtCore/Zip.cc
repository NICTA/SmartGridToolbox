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

#include "Zip.h"

#include <ostream>

namespace Sgt
{
    GenericZip::GenericZip(const std::string& id, const Phases& phases) :
        Component(id),
        ZipAbc(phases),
        YConst_(phases.size(), arma::fill::zeros),
        IConst_(phases.size(), arma::fill::zeros),
        SConst_(phases.size(), arma::fill::zeros)
    {
        // Empty.
    }

    json ZipAbc::asJson() const
    {
        json j = Component::asJson();
		j[sComponentType()] = {
			{"phases", toJson(phases())},
			{"YConst", toJson(YConst())},
			{"IConst", toJson(IConst())},
			{"SConst", toJson(SConst())}};
		return j;
	}
}
