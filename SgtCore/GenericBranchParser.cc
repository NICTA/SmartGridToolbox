#include "GenericBranchParser.h"

#include "GenericBranch.h"
#include "Network.h"

namespace SmartGridToolbox
{
   // No cross terms, just nPhase lines with single admittances.
   namespace {
      const ublas::matrix<Complex> YSimpleLine(const ublas::vector<Complex>& Y)
      {
         int nPhase = Y.size();
         int nTerm = 2 * nPhase;
         ublas::matrix<Complex> YNode(nTerm, nTerm, czero);
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

   void GenericBranchParser::parse(const YAML::Node& nd, Network& netw) const
   {
      SGT_DEBUG(debug() << "GenericBranch : parse." << std::endl);

      assertFieldPresent(nd, "id");
      assertFieldPresent(nd, "phases_0");
      assertFieldPresent(nd, "phases_1");
      assertFieldPresent(nd, "bus_0_id");
      assertFieldPresent(nd, "bus_1_id");
      assertFieldPresent(nd, "Y");

      std::string id = nd["id"].as<std::string>();
      Phases phases0 = nd["phases_0"].as<Phases>();
      Phases phases1 = nd["phases_1"].as<Phases>();
      
      std::string bus0Id = nd["bus_0_id"].as<std::string>();
      std::string bus1Id = nd["bus_1_id"].as<std::string>();

      const YAML::Node& ndY = nd["Y"];
      const YAML::Node& ndYMatrix = ndY["matrix"];
      const YAML::Node& ndYSimpleLine = ndY["simple_line"];
      ublas::matrix<Complex> Y(2 * phases0.size(), 2 * phases1.size(), czero);
      if (ndYMatrix)
      {
         Y = ndYMatrix.as<ublas::matrix<Complex>>();
      }
      else if (ndYSimpleLine)
      {
         ublas::vector<Complex> YLine = ndYSimpleLine.as<ublas::vector<Complex>>();
         Y = YSimpleLine(YLine);
      }

      std::unique_ptr<GenericBranch> branch(new GenericBranch(id, phases0, phases1));

      branch->setY(Y);

      netw.addArc(std::move(branch), bus0Id, bus1Id);
   }
}
