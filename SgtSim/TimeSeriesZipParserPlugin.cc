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
        assertFieldPresent(nd, "phases");
        assertFieldPresent(nd, "sim_network_id");
        assertFieldPresent(nd, "bus_id");
        assertFieldPresent(nd, "time_series_id");
        assertFieldPresent(nd, "dt");
        assertFieldPresent(nd, "matrix_elements");

        string id = parser.expand<std::string>(nd["id"]);
        Phases phases = parser.expand<Phases>(nd["phases"]);

        string networkId = parser.expand<std::string>(nd["sim_network_id"]);
        string busId = parser.expand<std::string>(nd["bus_id"]);
        
        // TODO: Would be better to force zip to be created explicitly, or
        // used from elsewhere.
        auto& simNetwork = *sim.simComponent<SimNetwork>(networkId);
        auto zip = simNetwork.network().addZip(std::make_shared<Zip>(id, phases), busId);
        simNetwork.linkZip(*zip);

        string tsId = parser.expand<std::string>(nd["time_series_id"]);
        ConstTimeSeriesPtr<TimeSeries<Time, arma::Col<Complex>>> series = 
            sim.timeSeries()[tsId].as<TimeSeries<Time, arma::Col<Complex>>, true>();

        Time dt = parser.expand<Time>(nd["dt"]);

        arma::Mat<arma::uword> matrixElems;
        YAML::Node ndMatElems = nd["matrix_elements"];
        if (ndMatElems)
        {
            matrixElems = parser.expand<arma::Mat<arma::uword>>(ndMatElems);
        }

        arma::Col<arma::uword> dataIdxsY;
        YAML::Node ndDataIdxsY = nd["data_indices_Y"];
        if (ndDataIdxsY)
        {
            dataIdxsY = parser.expand<arma::Col<arma::uword>>(ndDataIdxsY);
        }

        arma::Col<arma::uword> dataIdxsI;
        YAML::Node ndDataIdxsI = nd["data_indices_I"];
        if (ndDataIdxsI)
        {
            dataIdxsI = parser.expand<arma::Col<arma::uword>>(ndDataIdxsI);
        }

        arma::Col<arma::uword> dataIdxsS;
        YAML::Node ndDataIdxsS = nd["data_indices_S"];
        if (ndDataIdxsS)
        {
            dataIdxsS = parser.expand<arma::Col<arma::uword>>(ndDataIdxsS);
        }
        
        auto tsZip = sim.newSimComponent<TimeSeriesZip>(id, zip, series, dt, matrixElems, dataIdxsY, dataIdxsI,
                dataIdxsS);

        auto ndScaleFactorY = nd["scale_factor_Y"];
        if (ndScaleFactorY)
        {
            double scaleFactorY = parser.expand<double>(ndScaleFactorY);
            tsZip->setScaleFactorY(scaleFactorY);
        }

        auto ndScaleFactorI = nd["scale_factor_I"];
        if (ndScaleFactorI)
        {
            double scaleFactorI = parser.expand<double>(ndScaleFactorI);
            tsZip->setScaleFactorI(scaleFactorI);
        }

        auto ndScaleFactorS = nd["scale_factor_S"];
        if (ndScaleFactorS)
        {
            double scaleFactorS = parser.expand<double>(ndScaleFactorS);
            tsZip->setScaleFactorS(scaleFactorS);
        }
    }
}
