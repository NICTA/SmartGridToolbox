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

#include <SgtCore/Units.h>

namespace Units
{
    namespace SI
    {
        // Base SI units.
        const Unit<decltype(LDim())> m = {{LDim(), 1.0}, "m"};
        const Unit<decltype(MDim())> kg = {{MDim(), 1.0}, "kg"};
        const Unit<decltype(TDim())> s = {{TDim(), 1.0}, "s"};
        const Unit<decltype(IDim())> A = {{IDim(), 1.0}, "A"};
        const Unit<decltype(ThDim())> K = {{ThDim(), 1.0}, "K"};

        const Unit<decltype(QDim())> C = {{IDim() * TDim(), 1.0}, "C"};
    }
}
