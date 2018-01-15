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

#include "SimGlobalParserPlugin.h"

namespace Sgt
{
    namespace
    {
        Time parseTime(const YAML::Node& nd, const ParserBase& parser)
        {
            return timeFromLocalPtime(parser.expand<boost::posix_time::ptime>(nd));
        }
    }

    void SimGlobalParserPlugin::parse(const YAML::Node& nd, Simulation& sim, const ParserBase& parser) const
    {
        assertFieldPresent(nd, "start_time");
        assertFieldPresent(nd, "end_time");

        if (const YAML::Node& nodeTz = nd["timezone"])
        {
            try
            {
                timezone() = Timezone(parser.expand<std::string>(nodeTz));
            }
            catch (...)
            {
                sgtError("Couldn't parse timezone " << parser.expand<std::string>(nodeTz) << ".");
            }
        }

        const YAML::Node& nodeStart = nd["start_time"];
        try
        {
            sim.setStartTime(parseTime(nodeStart, parser));
        }
        catch (...)
        {
            sgtError("Couldn't parse start date " << parser.expand<std::string>(nodeStart) << ".");
        }

        const YAML::Node& nodeEnd = nd["end_time"];
        try
        {
            sim.setEndTime(parseTime(nodeEnd, parser));
        }
        catch (...)
        {
            sgtError("Couldn't parse end date " << parser.expand<std::string>(nodeEnd) << ".");
        }

        if (const YAML::Node& nodeLatLong = nd["lat_long"])
        {
            try
            {
                std::vector<double> llvec = parser.expand<std::vector<double>>(nodeLatLong);
                if (llvec.size() != 2)
                {
                    throw;
                };
                sim.setLatLong({llvec[0], llvec[1]});
            }
            catch (...)
            {
                sgtError("Couldn't parse lat_long " << parser.expand<std::string>(nodeLatLong) << ".");
            }
        }
    }

}
