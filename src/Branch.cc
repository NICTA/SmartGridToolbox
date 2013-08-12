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
      assertFieldPresent(nd, "bus_i");
      assertFieldPresent(nd, "bus_k");
      assertFieldPresent(nd, "Y");

      const std::string nameStr = nd["name"].as<std::string>();
      Branch & comp = mod.newComponent<Branch>(nameStr);

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

      const std::string busiStr = nd["bus_i"].as<std::string>();
      Bus * busiComp = mod.getComponentNamed<Bus>(busiStr);
      if (networkComp != nullptr)
      {
         comp->setBusi(*busiComp);
      }
      else
      {
         error() << "For component " << compNameStr <<  ", bus " << busiStr <<  " was not found in the model." 
               << std::endl;
         abort();
      }

      const std::string buskStr = nd["bus_k"].as<std::string>();
      Bus * buskComp = mod.getComponentNamed<Bus>(buskStr);
      if (networkComp != nullptr)
      {
         comp->setBusk(*buskComp);
      }
      else
      {
         error() << "For component " << compNameStr <<  ", bus " << buskStr <<  " was not found in the model." 
               << std::endl;
         abort();
      }
   }
}
