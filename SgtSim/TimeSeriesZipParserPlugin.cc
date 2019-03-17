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

#include "TimeSeriesZipParserPlugin.h"

#include "TimeSeriesZip.h"
#include "SimNetwork.h"
#include "Simulation.h"

namespace Sgt
{
    void TimeSeriesZipParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "bus_id");
        assertFieldPresent(nd, "sim_network_id");
        assertFieldPresent(nd, "time_series_id");
        assertFieldPresent(nd, "dt");
        assertFieldPresent(nd, "matrix_elements");

        std::string id = parser.expand<std::string>(nd["id"]);
        std::string busId = parser.expand<std::string>(nd["bus_id"]);
        auto ndZipId = nd["zip_id"];
        std::string zipId = ndZipId ? parser.expand<std::string>(ndZipId) : id;
        std::string simNetworkId = parser.expand<std::string>(nd["sim_network_id"]);

        auto& simNetwork = *sim.simComponent<SimNetwork>(simNetworkId);
        Network& network = simNetwork.network();
        ComponentPtr<Zip> zip = network.zips()[zipId]; 
        if (zip == nullptr)
        {
            assertFieldPresent(nd, "phases");
            Phases phases = parser.expand<Phases>(nd["phases"]);
            zip = network.addZip(std::make_shared<Zip>(zipId, phases), busId);
        }
        
        std::string tsId = parser.expand<std::string>(nd["time_series_id"]);
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
        
        auto tsZip = sim.newSimComponent<TimeSeriesZip>(id, zip, series, dt,
                matrixElems, dataIdxsY, dataIdxsI, dataIdxsS);
        simNetwork.addSimZip(tsZip);

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
