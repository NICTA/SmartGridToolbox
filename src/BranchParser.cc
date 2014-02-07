#include "BranchParser.h"

#include <SmartGridToolbox/Branch.h>
#include <SmartGridToolbox/Common.h>
#include <SmartGridToolbox/Network.h>

namespace SmartGridToolbox
{
   void BranchParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "Branch : parse." << std::endl);

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

      const YAML::Node & ndY = nd["Y"];
      const YAML::Node & ndYMatrix = ndY["matrix"];
      const YAML::Node & ndYSimpleLine = ndY["simple_line"];
      ublas::matrix<Complex> Y(2*phases0.size(), 2*phases1.size(), czero);
      if (ndYMatrix)
      {
         Y = ndYMatrix.as<ublas::matrix<Complex>>();
      }
      else if (ndYSimpleLine)
      {
         ublas::vector<Complex> y = ndYSimpleLine.as<ublas::vector<Complex>>();
         Y = YSimpleLine(y); 
      }

      Branch & comp = mod.newComponent<Branch>(name, phases0, phases1);
      comp.setY(Y);
   }

   void BranchParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "Branch : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      Branch* comp = mod.component<Branch>(name);

      const std::string networkStr = state.expandName(nd["network"].as<std::string>());
      Network* networkComp = mod.component<Network>(networkStr);
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
      const YAML::Node & ndY = nd["Y"];

      const std::string bus0Str = state.expandName(nd["bus_0"].as<std::string>());
      Bus* bus0Comp = mod.component<Bus>(bus0Str);
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
      Bus* bus1Comp = mod.component<Bus>(bus1Str);
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
