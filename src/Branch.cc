#include "Branch.h"
#include "Bus.h"
#include "Model.h"
#include "Network.h"
#include "PowerFlow.h"
 
namespace SmartGridToolbox
{
   void BranchParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "Branch : parse." << std::endl);
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "bus_0");
      assertFieldPresent(nd, "bus_1");
      assertFieldPresent(nd, "phases_0");
      assertFieldPresent(nd, "phases_1");
      assertFieldPresent(nd, "Y");

      Branch & comp = mod.newComponent<Branch>(state.curCompName());

      comp.phases0() = nd["phases_0"].as<Phases>();
      comp.phases1() = nd["phases_1"].as<Phases>();

      const YAML::Node & ndY = nd["Y"];
      const YAML::Node & ndYMatrix = ndY["matrix"];
      const YAML::Node & ndYSimpleLine = ndY["simple_line"];
      if (ndYMatrix)
      {
         comp.Y() = ndYMatrix.as<UblasMatrix<Complex>>();
      }
      else if (ndYSimpleLine)
      {
         UblasVector<Complex> y = ndYSimpleLine.as<UblasVector<Complex>>();
         UblasMatrix<Complex> Y = YSimpleLine(y); 
         comp.Y() = Y;
      }
   }

   void BranchParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "Branch : postParse." << std::endl);
      Branch * comp = mod.componentNamed<Branch>(state.curCompName());

      const std::string networkStr = state.expandName(nd["network"].as<std::string>());
      Network * networkComp = mod.componentNamed<Network>(networkStr);
      if (networkComp != nullptr)
      {
         networkComp->addBranch(*comp);
      }
      else
      {
         error() << "For component " << state.curCompName() <<  ", network " << networkStr 
                 <<  " was not found in the model." << std::endl;
         abort();
      }

      const std::string bus0Str = state.expandName(nd["bus_0"].as<std::string>());
      Bus * bus0Comp = mod.componentNamed<Bus>(bus0Str);
      if (networkComp != nullptr)
      {
         comp->setBus0(*bus0Comp);
      }
      else
      {
         error() << "For component " << state.curCompName() <<  ", bus " << bus0Str 
                 <<  " was not found in the model." << std::endl;
         abort();
      }

      const std::string bus1Str = state.expandName(nd["bus_1"].as<std::string>());
      Bus * bus1Comp = mod.componentNamed<Bus>(bus1Str);
      if (networkComp != nullptr)
      {
         comp->setBus1(*bus1Comp);
      }
      else
      {
         error() << "For component " << state.curCompName() <<  ", bus " << bus1Str 
                 <<  " was not found in the model." << std::endl;
         abort();
      }
   }
}
