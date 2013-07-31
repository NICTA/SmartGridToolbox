#include "Branch1P.h"
#include "Bus1P.h"
#include "Model.h"
#include "Network1P.h"
 
namespace SmartGridToolbox
{
   void Branch1PParser::parse(const YAML::Node & nd, Model & mod) const
   {
      SGTDebug("Branch1P : parse.");
      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "bus_i");
      assertFieldPresent(nd, "bus_k");
      assertFieldPresent(nd, "Y");

      Branch1P * comp = new Branch1P;
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

   void Branch1PParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      SGTDebug("Branch1P : postParse.");
      const std::string compNameStr = nd["name"].as<std::string>();
      Branch1P * comp = mod.getComponentNamed<Branch1P>(compNameStr);

      const std::string networkStr = nd["network"].as<std::string>();
      Network1P * networkComp = mod.getComponentNamed<Network1P>(networkStr);
      if (networkComp != nullptr)
      {
         comp->setNetwork(*networkComp);
         networkComp->addBranch(*comp);
      }
      else
      {
         SGTError("For component " << compNameStr <<  ", network " << networkStr <<  " was not found in the model.");
      }

      const std::string busiStr = nd["bus_i"].as<std::string>();
      Bus1P * busiComp = mod.getComponentNamed<Bus1P>(busiStr);
      if (networkComp != nullptr)
      {
         comp->setBusi(*busiComp);
      }
      else
      {
         SGTError("For component " << compNameStr <<  ", bus " << busiStr <<  " was not found in the model.");
      }

      const std::string buskStr = nd["bus_k"].as<std::string>();
      Bus1P * buskComp = mod.getComponentNamed<Bus1P>(buskStr);
      if (networkComp != nullptr)
      {
         comp->setBusk(*buskComp);
      }
      else
      {
         SGTError("For component " << compNameStr <<  ", bus " << buskStr <<  " was not found in the model.");
      }
   }
}
