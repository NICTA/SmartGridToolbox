#include "Bus1P.h"
#include "Model.h"
#include "Network1P.h"
 
namespace SmartGridToolbox
{
   void Bus1PParser::parse(const YAML::Node & nd, Model & mod) const
   {
      debug("Bus1P : parse.");
      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "type");

      Bus1P * comp = new Bus1P;
      const std::string nameStr = nd["name"].as<std::string>();
      comp->setName(nameStr);

      const std::string typeStr = nd["type"].as<std::string>();
      if (typeStr == "SL") 
      {
         comp->setType(BusType::SL);
      }
      else if (typeStr == "PQ") 
      {
         comp->setType(BusType::PQ);
      }
      else if (typeStr == "PV") 
      {
         comp->setType(BusType::PV);
      }
      else
      { 
         error("Bus type %s is not supported.", typeStr.c_str());
      }

      if (nd["slack_voltage"])
      {
         comp->setV(nd["slack_voltage"].as<Complex>());
      }

      if (nd["constant_impedance_load"])
      {
         comp->setY(nd["constant_impedance_load"].as<Complex>());
      }

      if (nd["constant_current_load"])
      {
         comp->setI(nd["constant_current_load"].as<Complex>());
      }

      if (nd["constant_power_load"])
      {
         comp->setS(nd["constant_power_load"].as<Complex>());
      }

      if (nd["generator_power"])
      {
         comp->setSGen(nd["generator_power"].as<Complex>());
      }

      mod.addComponent(*comp);
   }

   void Bus1PParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      debug("Bus1P : postParse.");
      const std::string compNameStr = nd["name"].as<std::string>();
      Bus1P * comp = mod.getComponentNamed<Bus1P>(compNameStr);

      const std::string networkStr = nd["network"].as<std::string>();
      Network1P * networkComp = mod.getComponentNamed<Network1P>(networkStr);
      if (networkComp != nullptr)
      {
         comp->setNetwork(*networkComp);
         networkComp->addBus(*comp);
      }
      else
      {
         error("For component %s, network %s was not found in the model.");
      }
   }
}
