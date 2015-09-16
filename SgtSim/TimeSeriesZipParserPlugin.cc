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

#include "TimeSeriesZipParserPlugin.h"

#include "TimeSeriesZip.h"
#include "SimNetwork.h"
#include "Simulation.h"

namespace Sgt
{
    void TimeSeriesZipParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "sim_network_id");
        assertFieldPresent(nd, "bus_id");
        assertFieldPresent(nd, "time_series_id");
        assertFieldPresent(nd, "phases");
        assertFieldPresent(nd, "dt");

        string id = parser.expand<std::string>(nd["id"]);
        string networkId = parser.expand<std::string>(nd["sim_network_id"]);
        string busId = parser.expand<std::string>(nd["bus_id"]);
        string tsId = parser.expand<std::string>(nd["time_series_id"]);
        Phases phases = parser.expand<Phases>(nd["phases"]);
        Time dt = parser.expand<Time>(nd["dt"]);

        auto ndScaleFactorY = nd["scale_factor_Y"];
        auto ndScaleFactorI = nd["scale_factor_I"];
        auto ndScaleFactorS = nd["scale_factor_S"];

        auto series = sim.timeSeries<const TimeSeries<Time, arma::Col<Complex>>>(tsId);
        auto network = sim.simComponent<SimNetwork>(networkId);
        auto tsZip = sim.newSimComponent<TimeSeriesZip>(id, phases, series, dt);
        if (ndScaleFactorY)
        {
            double scaleFactorY = parser.expand<double>(ndScaleFactorY);
            tsZip->setScaleFactorY(scaleFactorY);
        }
        if (ndScaleFactorI)
        {
            double scaleFactorI = parser.expand<double>(ndScaleFactorI);
            tsZip->setScaleFactorI(scaleFactorI);
        }
        if (ndScaleFactorS)
        {
            double scaleFactorS = parser.expand<double>(ndScaleFactorS);
            tsZip->setScaleFactorS(scaleFactorS);
        }

        network->network()->addZip(tsZip->zip(), busId);
        tsZip->linkToSimNetwork(*network);
    }
}
