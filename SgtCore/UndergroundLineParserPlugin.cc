#include "UndergroundLineParserPlugin.h"

#include "UndergroundLine.h"
#include "Network.h"
#include "YamlSupport.h"

namespace Sgt
{
    void UndergroundLineParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
    {
        auto ugl = parseUndergroundLine(nd, parser);

        assertFieldPresent(nd, "bus_0_id");
        assertFieldPresent(nd, "bus_1_id");

        std::string bus0Id = parser.expand<std::string>(nd["bus_0_id"]);
        std::string bus1Id = parser.expand<std::string>(nd["bus_1_id"]);

        netw.addBranch(std::move(ugl), bus0Id, bus1Id);
    }

    std::unique_ptr<UndergroundLine> UndergroundLineParserPlugin::parseUndergroundLine(const YAML::Node& nd,
            const ParserBase& parser) const
    {
        assertFieldPresent(nd, "id");
        assertFieldPresent(nd, "length");
        assertFieldPresent(nd, "has_neutral");
        assertFieldPresent(nd, "distance_matrix");
        assertFieldPresent(nd, "gmr_phase");
        assertFieldPresent(nd, "R_per_L_phase");
        assertFieldPresent(nd, "gmr_conc_strand");
        assertFieldPresent(nd, "R_per_L_conc_strand");
        assertFieldPresent(nd, "n_conc_strands");
        assertFieldPresent(nd, "radius_conc");
        assertFieldPresent(nd, "earth_resistivity");
        assertFieldPresent(nd, "freq");

        string id = parser.expand<std::string>(nd["id"]);
        double length = parser.expand<double>(nd["length"]);
        bool hasNeutral = parser.expand<bool>(nd["has_neutral"]);
        arma::Mat<double> distMatrix = parser.expand<arma::Mat<double>>(nd["distance_matrix"]);
        double gmrPhase = parser.expand<double>(nd["gmr_phase"]);
        double RPerLPhase = parser.expand<double>(nd["R_per_L_phase"]);
        double gmrConcStrand = parser.expand<double>(nd["gmr_conc_strand"]);
        double RPerLConcStrand = parser.expand<double>(nd["R_per_L_conc_strand"]);
        int nConcStrands = parser.expand<int>(nd["n_conc_strands"]);
        double rConc = parser.expand<double>(nd["radius_conc"]);
        double earthResistivity = parser.expand<double>(nd["earth_resistivity"]);
        double freq = parser.expand<double>(nd["freq"]);

        std::unique_ptr<UndergroundLine> ugl(new UndergroundLine(id, length, hasNeutral, distMatrix,
                                             gmrPhase, RPerLPhase, gmrConcStrand, RPerLConcStrand,
                                             nConcStrands, rConc, earthResistivity, freq));
        return ugl;
    }
}
