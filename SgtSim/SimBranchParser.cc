#include "SimBranchParser.h"

#include "SimBranch.h"
#include "SimBus.h"
#include "SimNetwork.h"

#include <SgtCore/Common.h>

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

   void SimBranchParser::parse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "SimBranch : parse." << std::endl);

      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "bus_0");
      assertFieldPresent(nd, "bus_1");
      assertFieldPresent(nd, "phases_0");
      assertFieldPresent(nd, "phases_1");
      assertFieldPresent(nd, "Y");

      string name = state.expandName(nd["name"].as<std::string>());
      Phases phases0 = nd["phases_0"].as<Phases>();
      Phases phases1 = nd["phases_1"].as<Phases>();

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

      SimBranch& comp = mod.newComponent<SimBranch>(name, phases0, phases1);
      comp.setY(Y);
   }

   void SimBranchParser::postParse(const YAML::Node& nd, Model& mod, const ParserState& state) const
   {
      SGT_DEBUG(debug() << "SimBranch : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      SimBranch* comp = mod.component<SimBranch>(name);

      const std::string networkStr = state.expandName(nd["network"].as<std::string>());
      SimNetwork* networkComp = mod.component<SimNetwork>(networkStr);
      if (networkComp != nullptr)
      {
         networkComp->addBranch(*comp);
      }
      else
      {
         error() << "For component " << name <<  ", network " << networkStr
                 <<  " was not found in the model." << std::endl;
         abort();
      }

      // Depends on network freq, so do in postParse.
      const YAML::Node& ndY = nd["Y"];

      const std::string bus0Str = state.expandName(nd["bus_0"].as<std::string>());
      SimBus* bus0Comp = mod.component<SimBus>(bus0Str);
      if (networkComp != nullptr)
      {
         comp->setBus0(*bus0Comp);
      }
      else
      {
         error() << "For component " << name <<  ", bus " << bus0Str
                 <<  " was not found in the model." << std::endl;
         abort();
      }

      const std::string bus1Str = state.expandName(nd["bus_1"].as<std::string>());
      SimBus* bus1Comp = mod.component<SimBus>(bus1Str);
      if (networkComp != nullptr)
      {
         comp->setBus1(*bus1Comp);
      }
      else
      {
         error() << "For component " << name <<  ", bus " << bus1Str
                 <<  " was not found in the model." << std::endl;
         abort();
      }
   }
}
