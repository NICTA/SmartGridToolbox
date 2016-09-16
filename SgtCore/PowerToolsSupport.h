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

#ifndef POWER_TOOLS_SUPPORT_DOT_H
#define POWER_TOOLS_SUPPORT_DOT_H

#include <SgtCore/Network.h>
#include <SgtCore/PowerFlowSolver.h>

class Model;
class Net;
class PowerModel;

#include <memory>

namespace Sgt
{
    class Network;

    Net* sgt2PowerTools(const Network& sgtNw);
    void powerTools2Sgt(const Net& ptNetw, Network& sgtNw);
    void printNetw(const Net& net);
    void printModel(const Model& mod);
}

#endif // POWER_TOOLS_SUPPORT_DOT_H
