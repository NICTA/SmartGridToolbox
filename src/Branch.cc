#include "Branch.h"
#include "Bus.h"
#include "Model.h"
#include "Network.h"
 
namespace SmartGridToolbox
{
   void BranchParser::parse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "Branch : parse." << std::endl);
      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "bus_0");
      assertFieldPresent(nd, "bus_1");
      assertFieldPresent(nd, "phases_0");
      assertFieldPresent(nd, "phases_1");
      assertFieldPresent(nd, "Y");

      const std::string nameStr = nd["name"].as<std::string>();
      Branch & comp = mod.newComponent<Branch>(nameStr);

      comp.setPhases0(nd["phases_0"].as<Phases>());
      comp.setPhases1(nd["phases_1"].as<Phases>());
      comp.setY(nd["Y"].as<UblasMatrix<Complex>>());
   }

   void BranchParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "Branch : postParse." << std::endl);
      const std::string compNameStr = nd["name"].as<std::string>();
      Branch * comp = mod.getComponentNamed<Branch>(compNameStr);

      const std::string networkStr = nd["network"].as<std::string>();
      Network * networkComp = mod.getComponentNamed<Network>(networkStr);
      if (networkComp != nullptr)
      {
         networkComp->addBranch(*comp);
      }
      else
      {
         error() << "For component " << compNameStr <<  ", network " << networkStr <<  " was not found in the model." 
               << std::endl;
         abort();
      }

      const std::string bus0Str = nd["bus_0"].as<std::string>();
      Bus * bus0Comp = mod.getComponentNamed<Bus>(bus0Str);
      if (networkComp != nullptr)
      {
         comp->setBus0(*bus0Comp);
      }
      else
      {
         error() << "For component " << compNameStr <<  ", bus " << bus0Str <<  " was not found in the model." 
               << std::endl;
         abort();
      }

      const std::string bus1Str = nd["bus_1"].as<std::string>();
      Bus * bus1Comp = mod.getComponentNamed<Bus>(bus1Str);
      if (networkComp != nullptr)
      {
         comp->setBus1(*bus1Comp);
      }
      else
      {
         error() << "For component " << compNameStr <<  ", bus " << bus1Str <<  " was not found in the model." 
               << std::endl;
         abort();
      }
   }
}
