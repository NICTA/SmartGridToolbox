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

#include "TimeSeriesDcPowerSourceParserPlugin.h"

#include "Inverter.h"
#include "TimeSeriesDcPowerSource.h"
#include "SimNetwork.h"
#include "Simulation.h"

using namespace std;

namespace Sgt
{
    void TimeSeriesDcPowerSourceParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "time_series_id");
        assertFieldPresent(nd, "inverter_id");
        assertFieldPresent(nd, "dt");

        string id = parser.expand<string>(nd["id"]);
        string timeSeriesId = nd["time_series_id"].as<string>();
        string inverterId = nd["inverter_id"].as<string>();
        
        auto ndScaleFactor = nd["scaleFactor"];
        double scaleFactor = ndScaleFactor ? parser.expand<double>(ndScaleFactor) : 1.0;
        Time dt = parser.expand<Time>(nd["dt"]);

        auto series = sim.timeSeries()[timeSeriesId].as<TimeSeries<Time, double>>();
        auto comp = sim.newSimComponent<TimeSeriesDcPowerSource>(id, series, scaleFactor, dt);

        auto inverterComp = sim.simComponent<InverterAbc>(inverterId);
        sgtAssert(inverterComp != nullptr,
                "For component " << id << ", inverter " << inverterId << " was not found in the simulation.");
        inverterComp->addDcPowerSource(comp);
    }
}
