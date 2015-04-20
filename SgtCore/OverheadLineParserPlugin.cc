#include "OverheadLineParserPlugin.h"

#include "OverheadLine.h"
#include "Network.h"
#include "YamlSupport.h"

namespace Sgt
{
    void OverheadLineParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
    {
        auto ohl = parseOverheadLine(nd, parser);

        assertFieldPresent(nd, "bus_0_id");
        assertFieldPresent(nd, "bus_1_id");

        std::string bus0Id = parser.expand<std::string>(nd["bus_0_id"]);
        std::string bus1Id = parser.expand<std::string>(nd["bus_1_id"]);

        netw.addBranch(std::move(ohl), bus0Id, bus1Id);
    }

    std::unique_ptr<OverheadLine> OverheadLineParserPlugin::parseOverheadLine(const YAML::Node& nd,
            const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "phases_0");
        assertFieldPresent(nd, "phases_1");
        assertFieldPresent(nd, "length");
        assertFieldPresent(nd, "cond_dist");
        assertFieldPresent(nd, "subcond_gmr");
        assertFieldPresent(nd, "subcond_R_per_L");
        assertFieldPresent(nd, "freq");
        assertFieldPresent(nd, "earth_resist");

        string id = parser.expand<std::string>(nd["id"]);
        Phases phases0 = parser.expand<Phases>(nd["phases_0"]);
        Phases phases1 = parser.expand<Phases>(nd["phases_1"]);
        double L = parser.expand<double>(nd["length"]);
        arma::Mat<double> condDist = parser.expand<arma::Mat<double>>(nd["cond_dist"]);
        arma::Col<double> subcondGmr = parser.expand<arma::Col<double>>(nd["subcond_gmr"]);
        arma::Col<double> subcondRPerL = parser.expand<arma::Col<double>>(nd["subcond_R_per_L"]);
        double freq = parser.expand<double>(nd["freq"]);
        double earthResist = parser.expand<double>(nd["earth_resist"]);

        arma::Col<int> nInBundle;
        arma::Col<double> adjSubcondDist;
        auto ndNInBundle = nd["n_in_bundle"];
        if (ndNInBundle)
        {
            nInBundle = parser.expand<arma::Col<int>>(ndNInBundle);
            adjSubcondDist = parser.expand<arma::Col<double>>(nd["adj_subcond_dist"]);
        }

        std::unique_ptr<OverheadLine> ohl(new OverheadLine(id, phases0, phases1, L, condDist, subcondGmr,
                    subcondRPerL, freq, earthResist));

        return ohl;
    }
}
