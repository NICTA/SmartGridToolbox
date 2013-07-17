#include "BusComponent.h"
#include "Model.h"
 
namespace SmartGridToolbox
{
   void BusComponentParser::parse(const YAML::Node & nd, Model & mod) const
   {
      Bus1PComponent * comp = new Bus1PComponent;

      assertFieldPresent(nd, "name");
      const std::string nameStr = nd["name"].as<std::string>();
      comp->setName(nameStr);

      assertFieldPresent(nd, "type");
      const std::string typeStr = nd["type"].as<std::string>();
      switch (typeStr)
      {
         case "SL" : comp.setType(BusType::SL); break;
         case "PQ" : comp.setType(BusType::PQ); break;
         case "PV" : comp.setType(BusType::PV); break;
         default: error("Bus type %s is not supported.", typeStr); break;
      }

      if (nd["slack_voltage"])
      {
         const std::string s = nd["slack_voltage"].as<std::string>();
         Complex c; s >> c;
         comp->setV(c);
      }

      if (nd["constant_impedance_load"])
      {
         const std::string s = nd["constant_impedance_load"].as<std::string>();
         Complex c; s >> c;
         comp->setY(c);
      }

      if (nd["constant_current_load"])
      {
         const std::string s = nd["constant_current_load"].as<std::string>();
         Complex c; s >> c;
         comp->setI(c);
      }

      if (nd["constant_power_load"])
      {
         const std::string s = nd["constant_power_load"].as<std::string>();
         Complex c; s >> c;
         comp->setS(c);
      }

      if (nd["generator_power"])
      {
         const std::string s = nd["generator_power"].as<std::string>();
         Complex c; s >> c;
         comp->setSGen(c);
      }

      mod.addComponent(*bc);
   }

   void BusComponentParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      const std::string myName = nd["name"].as<std::string>();
      Bus1PComponent * busComp = mod.getComponentNamed<Bus1PComponent>(myName);
      const std::string networkName = nd["network_name"].as<std::string>();
      Network1PComponent * networkComp = mod.getComponentNamed<NetworkComponent>(networkName);
      if (networkComp != nullptr)
      {
         busComp->setNetwork(networkComp);
      }
      else
      {
         error("For component %s, network %s was not found in the model.");
      }
   }
}
