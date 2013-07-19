#include "Branch1PComponent.h"
#include "Bus1PComponent.h"
#include "Model.h"
#include "Network1PComponent.h"
 
namespace SmartGridToolbox
{
   void Branch1PComponentParser::parse(const YAML::Node & nd, Model & mod) const
   {
      debug("Branch1PComponent : parse.");
      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "bus_i");
      assertFieldPresent(nd, "bus_k");
      assertFieldPresent(nd, "Y");

      Branch1PComponent * comp = new Branch1PComponent;
      const std::string nameStr = nd["name"].as<std::string>();
      comp->setName(nameStr);

      UblasMatrix<Complex> YMat = nd["Y"].as<UblasMatrix<Complex>>();
      Array2D<Complex, 2, 2> Y;
      Y[0][0] = YMat(0, 0);
      Y[0][1] = YMat(0, 1);
      Y[1][0] = YMat(1, 0);
      Y[1][1] = YMat(1, 1);
      comp->setY(Y);

      mod.addComponent(*comp);
   }

   void Branch1PComponentParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      debug("Branch1PComponent : postParse.");
      const std::string compNameStr = nd["name"].as<std::string>();
      Branch1PComponent * comp = mod.getComponentNamed<Branch1PComponent>(compNameStr);

      const std::string networkStr = nd["network"].as<std::string>();
      Network1PComponent * networkComp = mod.getComponentNamed<Network1PComponent>(networkStr);
      if (networkComp != nullptr)
      {
         comp->setNetwork(*networkComp);
         networkComp->addBranch(*comp);
      }
      else
      {
         error("For component %s, network %s was not found in the model.");
      }

      const std::string busiStr = nd["bus_i"].as<std::string>();
      Bus1PComponent * busiComp = mod.getComponentNamed<Bus1PComponent>(busiStr);
      if (networkComp != nullptr)
      {
         comp->setBusi(*busiComp);
      }
      else
      {
         error("For component %s, bus_i %s was not found in the model.");
      }

      const std::string buskStr = nd["bus_k"].as<std::string>();
      Bus1PComponent * buskComp = mod.getComponentNamed<Bus1PComponent>(buskStr);
      if (networkComp != nullptr)
      {
         comp->setBusk(*buskComp);
      }
      else
      {
         error("For component %s, bus_k %s was not found in the model.");
      }
   }
}
