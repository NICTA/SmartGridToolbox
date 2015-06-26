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

#include "Parser.h"
#include "BusParserPlugin.h"
#include "CommonBranchParserPlugin.h"
#include "DgyTransformerParserPlugin.h"
#include "GenericBranchParserPlugin.h"
#include "GenericGenParserPlugin.h"
#include "GenericZipParserPlugin.h"
#include "MatpowerParserPlugin.h"
#include "OverheadLineParserPlugin.h"
#include "PowerFlowSolverParserPlugin.h"
#include "SinglePhaseTransformerParserPlugin.h"
#include "UndergroundLineParserPlugin.h"
#include "YyTransformerParserPlugin.h"

namespace Sgt
{
    class Network;
    template<> void registerParserPlugins<Network>(Parser<Network>& p)
    {
        p.registerParserPlugin<BusParserPlugin>();
        p.registerParserPlugin<CommonBranchParserPlugin>();
        p.registerParserPlugin<DgyTransformerParserPlugin>();
        p.registerParserPlugin<GenericBranchParserPlugin>();
        p.registerParserPlugin<GenericGenParserPlugin>();
        p.registerParserPlugin<GenericZipParserPlugin>();
        p.registerParserPlugin<MatpowerParserPlugin>();
        p.registerParserPlugin<OverheadLineParserPlugin>();
        p.registerParserPlugin<PowerFlowSolverParserPlugin>();
        p.registerParserPlugin<SinglePhaseTransformerParserPlugin>();
        p.registerParserPlugin<UndergroundLineParserPlugin>();
        p.registerParserPlugin<YyTransformerParserPlugin>();
    }
}
