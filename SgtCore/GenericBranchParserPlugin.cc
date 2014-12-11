#include "GenericBranchParserPlugin.h"

#include "Branch.h"
#include "Network.h"
#include "YamlSupport.h"

namespace SmartGridToolbox
{
   // No cross terms, just nPhase lines with single admittances.
   namespace {
      const arma::Mat<Complex> YSimpleLine(const arma::Col<Complex>& Y)
      {
         int nPhase = Y.size();
         int nTerm = 2 * nPhase;
         arma::Mat<Complex> YNode(nTerm, nTerm, arma::fill::zeros);
         for (int i = 0; i < nPhase; ++i)
         {
            YNode(i, i) = Y(i);
            YNode(i + nPhase, i + nPhase) = Y(i);
            YNode(i, i + nPhase) = -Y(i);
            YNode(i + nPhase, i) = -Y(i);
         }
         return YNode;
      }
   }

   void GenericBranchParserPlugin::parse(const YAML::Node& nd, Network& netw, const ParserBase& parser) const
   {
      auto gb = parseGenericBranch(nd, parser);

      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");

      std::string bus0Id = parser.expand<std::string>(nd["bus_0_id"]);
      std::string bus1Id = parser.expand<std::string>(nd["bus_1_id"]);
      
      netw.addArc(std::move(gb), bus0Id, bus1Id);
   }

   std::unique_ptr<GenericBranch> GenericBranchParserPlugin::parseGenericBranch(const YAML::Node& nd,
         const ParserBase& parser) const
   {
      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases_0");
      assertFieldPresent(nd, "phases_1");
      assertFieldPresent(nd, "Y");

      std::string id = parser.expand<std::string>(nd["id"]);
      Phases phases0 = parser.expand<Phases>(nd["phases_0"]);
      Phases phases1 = parser.expand<Phases>(nd["phases_1"]);
      
      const YAML::Node& ndY = nd["Y"];
      const YAML::Node& ndYMatrix = ndY["matrix"];
      const YAML::Node& ndYSimpleLine = ndY["simple_line"];
      arma::Mat<Complex> Y(2 * phases0.size(), 2 * phases1.size(), arma::fill::zeros);
      if (ndYMatrix)
      {
         Y = parser.expand<arma::Mat<Complex>>(ndYMatrix);
      }
      else if (ndYSimpleLine)
      {
         arma::Col<Complex> YLine = parser.expand<arma::Col<Complex>>(ndYSimpleLine);
         Y = YSimpleLine(YLine);
      }

      std::unique_ptr<GenericBranch> branch(new GenericBranch(id, phases0, phases1));

      branch->setInServiceY(Y);

      return branch;
   }
}
