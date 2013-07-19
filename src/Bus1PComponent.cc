#include "Bus1PComponent.h"
#include "Model.h"
#include "Network1PComponent.h"
 
namespace SmartGridToolbox
{
   void Bus1PComponentParser::parse(const YAML::Node & nd, Model & mod) const
   {
      debug("Bus1PComponent : parse.");
      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "type");

      Bus1PComponent * comp = new Bus1PComponent;
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

   void Bus1PComponentParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      debug("Bus1PComponent : postParse.");
      const std::string compNameStr = nd["name"].as<std::string>();
      Bus1PComponent * comp = mod.getComponentNamed<Bus1PComponent>(compNameStr);

      const std::string networkStr = nd["network"].as<std::string>();
      Network1PComponent * networkComp = mod.getComponentNamed<Network1PComponent>(networkStr);
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
